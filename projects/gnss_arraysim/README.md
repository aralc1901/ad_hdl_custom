# GNSS ArraySim para FMCOMMS5/ZCU102

Proyecto derivado de `projects/fmcomms5` del mismo repositorio, revisión
`ae6e248f2`. Conserva el bloque de dos AD9361, el mapeo de FMC, la
sincronización, los relojes, GPIO/SPI, los FIFO de radio y ambos DMA.

## Estructura y archivos adaptados

Se copiaron los 20 archivos fuente versionados de `projects/fmcomms5`, incluidos
`common/`, `zc702/`, `zc706/`, `zcu102/` y la documentación original en
`Readme.md`. Los resultados locales de Vivado (`.xpr`, `.runs`, `.srcs`, logs,
XSA y otros) son regenerables y no se incorporaron al nuevo proyecto.

| Archivo del nuevo proyecto | Cambio sobre FMCOMMS5 |
| --- | --- |
| `common/gnss_arraysim_bd.tcl` | Copia de `common/fmcomms5_bd.tcl`, sin cambios funcionales; contiene la instanciación y conexiones de los dos AD9361. |
| `*/Makefile` | `PROJECT_NAME` pasa a `gnss_arraysim_<placa>` y la dependencia del bloque común apunta a la copia local. Las dependencias de IP ADI permanecen iguales. |
| `*/system_bd.tcl` | La fuente del bloque común apunta a `../common/gnss_arraysim_bd.tcl`; no cambia la topología de radio. |
| `*/system_project.tcl` | Las llamadas `adi_project`, `adi_project_files` y `adi_project_run` usan el nombre nuevo. |
| `*/system_top.v` y `*/system_constr.xdc` | Copias idénticas; conservan las señales, la sincronización y los pines originales. |
| `common/fmcomms5_fmc1.txt`, `common/fmcomms5_fmc2.txt` y `Readme.md` | Copias de la documentación y datos de referencia FMCOMMS5. |
| `zcu102/system_project.tcl` (integración IP) | Comprueba que existe `library/gnss_arraysim`, mantiene `library/` en `ip_repo_paths` y actualiza el catálogo. El flujo ADI ya registra `library/` antes de cargar `system_bd.tcl`. Omite el ajuste de retardos AD9361 sólo en modo `ADI_SKIP_SYNTHESIS`, cuando no hay diseño sintetizado abierto. |
| `zcu102/Makefile` (integración IP) | Añade los cuatro `component.xml` como dependencias de proyecto. |
| `zcu102/check_ip_catalog.tcl` | Comprueba los cuatro VLNV con un proyecto Vivado en memoria. |

La placa objetivo de la integración GNSS es ZCU102. Las copias de ZC702 y ZC706
se mantienen como referencia y sus nombres de proyecto se adaptaron para que
el árbol completo sea coherente; la integración de las IP GNSS se hará sólo en
`zcu102/`.

Los cuatro paquetes HLS completos, sus hashes y advertencias de procedencia
están en [`../../library/gnss_arraysim/README.md`](../../library/gnss_arraysim/README.md).
La primera prueba digital de `jam_tone_loop` se describe abajo. Las otras tres
IP siguen disponibles en el catálogo, sin instancias en el diseño.

## Primera prueba digital: tono a BRAM

```text
PS AXI ──┬── AXI4-Lite jam_tone_loop ── tone_out (32 bits) ── capturador ── BRAM puerto B
         ├── AXI GPIO (armado/estado) ────────────────────────────┘
         └── AXI BRAM Controller ── BRAM puerto A ── lectura PS/Linux

FMCOMMS5: 2 × AD9361 ↔ FIFO de radio ↔ DMA ADC y DMA DAC (sin cambios)
```

El capturador acepta sólo `TVALID && TREADY`, escribe una palabra alineada en
`4 × índice` y termina tras el número solicitado (1–1024). No necesita `TLAST`:
el límite es el contador de transferencias aceptadas. `TREADY` baja antes de
armar, durante pausa y después de completar. Reloj de `jam_tone_loop`, GPIO,
capturador, BRAM B y controlador AXI BRAM: `sys_cpu_clk` (100 MHz); reset
activo bajo `sys_cpu_resetn`. Se conecta `interrupt` del tono a `ps-14`; la
captura se consulta por estado GPIO. No hay salida RF ni ArrayGen en esta etapa.

### Paquete que usa Vivado

Se inspeccionó `library/gnss_arraysim/jam_tone_loop/component.xml`, el RTL
empaquetado, `drivers/jam_tone_loop_v1_0/src/xjam_tone_loop_hw.h` y las fuentes
externas. El catálogo devuelve exactamente
`xilinx.com:hls:jam_tone_loop:1.0`. Tiene AXI4-Lite `s_axi_control`, `ap_clk`,
`ap_rst_n`, `interrupt` y AXI4-Stream `tone_out` con `TDATA[31:0]`, `TVALID` y
`TREADY`; no tiene `TLAST`. Cada palabra es `{Q[15:0], I[15:0]}`: I en bits
15:0, Q en bits 31:16, ambos con signo en complemento a dos Q2.14. El RTL
empaqueta `sample_imag` arriba y `sample_real` abajo. La fuente HLS incrementa
un acumulador de fase estático **antes** de cada muestra y calcula I con coseno
y Q con seno. El acumulador conserva la fase entre ejecuciones hasta reset PL.
`n_samples` es de 32 bits, `amplitude` de 16 bits con signo Q2.14 y
`phase_step` de 32 bits sin signo. La salida usa un regslice HLS que propaga
`TREADY`; el pipeline detiene la producción cuando no puede aceptar otra
palabra. El test del capturador cubre la contrapresión; no se ha hecho una
cosimulación RTL del paquete HLS.

El ZIP importado tenía cambios locales en el origen. SHA-256:
`ff772f853e5c03484c249a56bdd75f83c9ce6a5806de8180c8735bbe4`.
El RTL principal importado coincide por SHA-256 con la solución HLS actual en
`gnss_arraysim_hdl/ip_cores/jam_tone_gen`, mientras que la fuente C++ no tiene
diff respecto a su HEAD. Los metadatos RTL indican exportación HLS 2022.2
para `xczu7eg-ffvc1156-2-e`; el destino aquí es
`xczu9eg-ffvb1156-2-e`. `check_tone_package.tcl` lo instanció y elaboró en
Vivado 2022.2 para XCZU9EG y el Block Design completo se validó. Esto verifica
compatibilidad de catálogo e IPI, pero aún no prueba síntesis ni timing para
ZCU102. No se modificó ni reexportó el paquete.

### Mapa AXI visto por el PS

Las direcciones que reciben las llamadas `ad_cpu_interconnect` en Tcl tienen
un desplazamiento de `0x40000000` en el mapa final ZynqMP. La segunda columna
es la que debe usar Linux; se comprobó en el `system.bd` generado.

| Bloque | Tcl ADI | PS/Linux | Rango |
| --- | --- | --- | --- |
| SYSID | `0x45000000` | `0x85000000` | 64 KiB |
| `jam_tone_0` | `0x47000000` | `0x87000000` | 64 KiB |
| BRAM de captura | `0x47010000` | `0x87010000` | 4 KiB |
| GPIO de captura | `0x47020000` | `0x87020000` | 4 KiB |
| AD9361 0 | `0x79020000` | `0x99020000` | 64 KiB |
| AD9361 1 | `0x79040000` | `0x99040000` | 64 KiB |
| DMA ADC | `0x7C400000` | `0x9C400000` | 64 KiB |
| DMA DAC | `0x7C420000` | `0x9C420000` | 64 KiB |

Los rangos no se solapan. IRQ existentes: DAC DMA `ps-12`, ADC DMA `ps-13`;
nueva IRQ tono `ps-14`.

### Registros y secuencia

Desplazamientos relativos a `0x87000000`: control HLS `0x00` (`ap_start` bit
0, `ap_done` bit 1 que se limpia al leer, `ap_idle` bit 2, `ap_ready` bit 3,
`auto_restart` bit 7), GIE `0x04`, IER `0x08`, ISR `0x0c`, `n_samples` `0x10`,
`amplitude` `0x18` (16 bits), `phase_step` `0x20`. Para una captura finita,
`auto_restart=0`.

El AXI GPIO en `0x87020000` tiene canal 1 DATA `0x00` y TRI `0x04`
(configurar TRI=0 para salida), canal 2 DATA `0x08` y TRI `0x0c`
(TRI=`0xffffffff` para entrada). Canal 1: bit 0 ARM por flanco ascendente,
bit 1 PAUSE, bits 26:16 longitud de 11 bits. El hardware rechaza 0 y
1025–2047. Canal 2: bits 10:0 número capturado, bit 11 DONE, bit 12 BUSY,
bit 13 longitud inválida, bit 14 rearme durante BUSY, bit 15 TREADY actual.
DONE y errores son persistentes hasta un rearme válido; un rearme durante BUSY
marca error y deja intacta la adquisición en marcha. Cada rearme válido pone
el contador a cero y sobrescribe desde la primera palabra de BRAM. El contenido
restante de BRAM no se borra.

Orden desde Linux: configurar TRI; confirmar `ap_idle=1`; poner `auto_restart=0`
y escribir `n_samples`, amplitud y paso de fase; escribir ARM 0→1→0 con la
longitud en 26:16; verificar BUSY=1 y errores=0; escribir `ap_start=1`;
esperar DONE y comprobar contador y errores; leer `0x87010000 + 4 × índice`
para `índice=0..n_samples-1`. Rearmar sólo cuando BUSY=0, antes de un nuevo
`ap_start`. El generador recibe el mismo `n_samples` que el capturador para
evitar un flujo residual. Su salida puede quedar en espera si se programa más
larga que la captura y requerir reset PL antes de repetir.

`capture_tone.py` automatiza la secuencia mediante `/dev/mem` y produce CSV:

```bash
sudo python3 projects/gnss_arraysim/zcu102/capture_tone.py \
  --samples 1024 --amplitude-raw 4096 --phase-step 42949673 \
  --phase-start 0 --sample-rate-hz 100000000 --output tone_capture.csv
```

Para usarla hacen falta el nuevo bitstream cargado, el mapa PL accesible al PS,
Linux con `/dev/mem` y permisos de acceso (o una interfaz UIO equivalente por
desarrollar). La secuencia no se ha ejecutado en placa. `--phase-start 0` sólo
es válido tras reset PL; para corridas sucesivas se usa el `next --phase-start`
que imprime el programa. El CSV incluye palabras hexadecimales, I/Q enteros y
valores Q2.14, además de referencia y error si se indica fase inicial.

La referencia independiente en Python calcula
`A × exp(j 2π (fase_inicial + (n+1) paso)/2^32)` en doble precisión, cuantiza
I/Q a Q2.14 y compara el error por componente en LSB. La cuantización HLS de
las funciones trigonométricas puede diferir, por lo que la tolerancia es un
criterio de prueba configurable, no una equivalencia bit a bit demostrada.
Se compara amplitud media `mean(abs(I+jQ))` con `abs(A)` y frecuencia mediante
la fase de `sum(conj(x[n]) × x[n+1])`, usando la **frecuencia real** de reloj
de muestras en `--sample-rate-hz`. También se comprueba continuidad de fase
comparando cada muestra con la referencia acumulada y, entre capturas, usando
la fase inicial propagada. La frecuencia nominal es
`phase_step × fs / 2^32`, módulo `fs` (aliasing si excede Nyquist).

### Archivos de esta etapa y validación

| Archivo | Función |
| --- | --- |
| `zcu102/system_bd.tcl` | Añade generador, GPIO, capturador y BRAM; conserva el bloque común de FMCOMMS5. |
| `zcu102/Makefile` | Declara `tone_capture.v` como dependencia de build. |
| `zcu102/tone_capture.v` | Cuenta transferencias, escribe BRAM y publica estado. |
| `zcu102/tb_tone_capture.sv` | Test focalizado de orden, longitud, contrapresión, errores y rearme. |
| `zcu102/check_tone_package.tcl` | Comprueba VLNV y puertos en XCZU9EG sin síntesis. |
| `zcu102/capture_tone.py` | Procedimiento de control y lectura Linux; pendiente de prueba en placa. |

Comandos de esta etapa desde la raíz:

```bash
make -n gnss_arraysim.zcu102
cd /tmp
/tools/Xilinx/Vivado/2022.2/bin/vivado -mode batch \
  -source /home/clara/fpga/git/ad_hdl_custom/projects/gnss_arraysim/zcu102/check_tone_package.tcl
cd /home/clara/fpga/git/ad_hdl_custom/projects/gnss_arraysim/zcu102
mkdir -p gnss_tone_validation4
ADI_PROJECT_DIR=gnss_tone_validation4/ ADI_SKIP_SYNTHESIS=1 \
  /tools/Xilinx/Vivado/2022.2/bin/vivado -mode batch -source system_project.tcl \
  -log gnss_tone_validation4/vivado.log -journal gnss_tone_validation4/vivado.jou
cd /tmp
/tools/Xilinx/Vivado/2022.2/bin/xvlog -sv \
  /home/clara/fpga/git/ad_hdl_custom/projects/gnss_arraysim/zcu102/tone_capture.v \
  /home/clara/fpga/git/ad_hdl_custom/projects/gnss_arraysim/zcu102/tb_tone_capture.sv
/tools/Xilinx/Vivado/2022.2/bin/xelab tb_tone_capture -s tb_tone_capture_sim
/tools/Xilinx/Vivado/2022.2/bin/xsim tb_tone_capture_sim -runall
```

`make -n` terminó con código 0 en esta etapa. La prueba del paquete terminó
con código 0 y mostró el VLNV y puertos esperados. La simulación XSim del
capturador pasó (1024 palabras, orden, pausa, backpressure, longitudes inválidas
y rearme). La validación de `system.bd` en Vivado 2022.2 terminó con código 0,
sin síntesis, en un directorio nuevo para conservar el build anterior. Emitió
una advertencia de ancho al unir `bram_addr_a` de 12 bits con `addra` de 32:
el wrapper generado extiende con ceros los bits superiores. La memoria generada
usa puertos de 32 bits y un segmento AXI de 4 KiB. La advertencia crítica sobre
el Tcl Store de usuario es de permisos del entorno; Vivado usó el almacén de
instalación. Siguen las advertencias de la base FMCOMMS5. Quedan pendientes
síntesis, implementación, timing, bitstream y prueba Linux/placa.

La dirección BRAM sigue el formato de byte del controlador: para 32 bits y
4 KiB, AMD documenta que el índice de 1024 palabras ocupa `BRAM_Addr[11:2]`
([PG078, tabla de direcciones BRAM](https://docs.amd.com/api/khub/documents/B6Uh5gbO0aGleXW3kIcXTA/content)).

El usuario informó que, tras resolver la licencia, `make gnss_arraysim.zcu102`
terminó en `OK` con `ADI_MAX_OOC_JOBS=1` **antes** de esta integración digital.
No hemos repetido ese build completo con los cambios actuales.

## Direcciones AXI del diseño base (históricas)

Proceden de `common/gnss_arraysim_bd.tcl` y del diseño base de ZCU102. Son las
direcciones dadas a las funciones Tcl ADI; para acceso desde PS usar la tabla
anterior.

| IP | Dirección base |
| --- | --- |
| `axi_sysid_0` | `0x45000000` |
| `axi_ad9361_0` | `0x79020000` |
| `axi_ad9361_1` | `0x79040000` |
| `axi_ad9361_adc_dma` | `0x7C400000` |
| `axi_ad9361_dac_dma` | `0x7C420000` |

IRQ originales: DAC DMA `ps-12`; ADC DMA `ps-13`.

## Comandos y registro de resultados

Desde la raíz de `ad_hdl_custom`:

```bash
make -n gnss_arraysim.zcu102
PATH=/tools/Xilinx/Vivado/2022.2/bin:$PATH make gnss_arraysim.zcu102
cd projects/gnss_arraysim/zcu102
PATH=/tools/Xilinx/Vivado/2022.2/bin:$PATH vivado -mode batch -source check_ip_catalog.tcl
ADI_SKIP_SYNTHESIS=1 PATH=/tools/Xilinx/Vivado/2022.2/bin:$PATH vivado -mode batch -source system_project.tcl > gnss_array_bd_check.log 2>&1
```

- `make -n gnss_arraysim.zcu102`: pasó y resolvió el proyecto y sus nueve
  dependencias ADI. El primer intento detectó que el bloque común aún no se
  había renombrado; se corrigieron el nombre y sus tres referencias y se repitió
  con resultado correcto. Se repitió tras añadir las cuatro dependencias HLS y
  siguió resolviendo el objetivo.
- La compilación Vivado 2022.2 del esqueleto creó y validó `system.bd`, pero
  falló al iniciar síntesis: `[Common 17-345] A valid license was not found for
  feature 'Synthesis' and/or device 'xczu9eg'`. No se obtuvo XSA. Vivado
  también emitió warnings de polaridad de reset FIFO y anchura de AXI en el
  diseño base. Log local: `zcu102/gnss_arraysim_zcu102_vivado.log` (generado e
  ignorado por Git). La prueba del catálogo se registra a continuación.
- `check_ip_catalog.tcl`: pasó en Vivado 2022.2 y encontró exactamente una
  definición de cada uno de los cuatro VLNV.
- El primer intento con `ADI_SKIP_SYNTHESIS=1` validó el Block Design, pero
  falló al ejecutar `axi_ad9361_delay.tcl`, que requiere un diseño sintetizado
  abierto. Se añadió la condición indicada arriba y se repitió la prueba:
  terminó con código 0, `system.bd` válido y sin errores Vivado. Log local:
  `zcu102/gnss_array_bd_check.log` (generado e ignorado por Git).
- Validación de hardware: pendiente.

El plan y el inventario de las IP externas están en
[`../../docs/gnss_arraysim_zcu102_plan.md`](../../docs/gnss_arraysim_zcu102_plan.md).
