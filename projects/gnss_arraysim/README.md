# GNSS ArraySim para FMCOMMS5/ZCU102

La imagen Linux de laboratorio y sus scripts de preparación, compilación y
empaquetado se encuentran en [`petalinux/README.md`](petalinux/README.md).
Incluye las capturas Python y soporte ADI/IIO para ZCU102 + FMCOMMS5.

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
Las pruebas digitales de `jam_tone_loop`, `jam_gaussian_loop` y
`gnss_prn_loop` se describen abajo. `fp_arraygen_loop` también está instanciado
en el Block Design con registros visibles desde PS; su camino de datos queda
estacionado hasta incorporar adaptadores FIFO y un consumidor de 128 bits.

## Compilación: validación por defecto

Desde la raíz del repositorio, `make gnss_arraysim.zcu102` prepara las
dependencias ADI, crea el proyecto y valida el Block Design. Por defecto **no
lanza síntesis, implementación ni generación de bitstream**. Escribe sus
resultados en `projects/gnss_arraysim/zcu102/gnss_bd_validation/`, separado
del build completo anterior, y deja allí `console.log` y `vivado.log`.

```bash
cd ~/fpga/git/ad_hdl_custom
PATH="/tools/Xilinx/Vivado/2022.2/bin:$PATH" make gnss_arraysim.zcu102
```

Para ejecutar el flujo ADI completo y generar el bitstream y el XSA, activar
explícitamente el flag **como variable de entorno delante de `make`**:

```bash
cd ~/fpga/git/ad_hdl_custom
GNSS_FULL_BUILD=1 ADI_MAX_OOC_JOBS=1 \
  PATH="/tools/Xilinx/Vivado/2022.2/bin:$PATH" make gnss_arraysim.zcu102
```

`GNSS_FULL_BUILD` sólo admite `0` (valor predeterminado) y `1`. El build
completo sigue la receta ADI de `project-xilinx.mk`, que regenera los
resultados de Vivado en el directorio habitual. La invocación directa de
`system_project.tcl` también omite síntesis por defecto. No combinar
`GNSS_FULL_BUILD=1` con `ADI_SKIP_SYNTHESIS`. `make -n` muestra la receta
correspondiente sin ejecutarla. `make all` dentro de `zcu102/` sin el flag se
rechaza antes de la receta que limpia resultados. El resto de proyectos ADI no
cambia.

Comprobación propia del 2026-11-11: `make -n gnss_arraysim.zcu102` mostró la
receta de validación BD; `GNSS_FULL_BUILD=1 make -n gnss_arraysim.zcu102`
mostró la receta ADI de síntesis e implementación. Se ejecutó realmente el
`make` predeterminado con Vivado 2022.2: código 0, `validate_bd_design`
satisfactorio y mensaje `Skipping synthesis` en
`gnss_bd_validation/console.log`. No se generaron `.bit` ni `.xsa` en ese
directorio. **El build completo con el flag no se ha ejecutado**; queda a
decisión del usuario.

## Capturas digitales: tono, ruido y PRN a BRAM

```text
PS AXI ──┬── AXI4-Lite jam_tone_loop ── tone_out ── capturador T ── BRAM T puerto B
         ├── GPIO T (armado/estado) ───────────────────────────────┘
         ├── AXI BRAM Controller T ── BRAM T puerto A ── PS/Linux
         ├── AXI4-Lite jam_gaussian_loop ── noise_out ── capturador G ── BRAM G puerto B
         ├── GPIO G (armado/estado) ─────────────────────────────────┘
         ├── AXI BRAM Controller G ── BRAM G puerto A ── PS/Linux
         ├── AXI4-Lite gnss_prn_loop (RAM chips) ── prn_out ── capturador P ── BRAM P puerto B
         ├── GPIO P (armado/estado) ──────────────────────────────────────────┘
         ├── AXI BRAM Controller P ── BRAM P puerto A ── PS/Linux
         └── AXI4-Lite fp_arraygen_loop (ap_start=0, FIFO vacíos)

FMCOMMS5: 2 × AD9361 ↔ FIFO de radio ↔ DMA ADC y DMA DAC (sin cambios)
```

Cada instancia de `stream_capture` acepta sólo `TVALID && TREADY`, escribe una
palabra alineada en `4 × índice` y termina tras el número solicitado (1–1024).
No necesita `TLAST`:
el límite es el contador de transferencias aceptadas. `TREADY` baja antes de
armar, durante pausa y después de completar. Los generadores, GPIO,
capturadores, BRAM B y controladores AXI BRAM usan `sys_cpu_clk` (100 MHz); reset
activo bajo `sys_cpu_resetn`. Se conecta `interrupt` del tono a `ps-14`; la
captura se consulta por estado GPIO. Ruido y PRN comparten `ps-15` a través
de una OR; las fuentes se distinguen mediante sus ISR HLS si se habilitan las
IRQ. ArrayGen no alimenta la salida RF en esta etapa.

La interfaz de escritura del capturador sólo expone `capture_write_en` y
`capture_write_addr[31:0]`. En la dirección sólo los bits 11:0 pueden variar;
los demás son cero porque el generador BRAM exige un puerto de 32 bits.
`capture_write_en` habilita el puerto B y sus cuatro bytes de escritura.
`tone_out_TDATA`, `noise_out_TDATA` y `prn_out_TDATA` llegan directamente a sus respectivos
puertos `dinb`; los puertos de lectura
pertenecen al controlador AXI BRAM.

Vivado confundía los nombres anteriores de los puertos RTL con interfaces del
catálogo ADI: creaba `bram_0`…`bram_31` de tipo
`analog.com:interface:jesd204_rx_ilas_config` a partir de un solo bus de
dirección de 32 bits, además de agrupar señales `VALID` y `EN`. Eran
**interfaces inferidas ficticias**, no lógica de captura ni puertos Verilog
adicionales. Los puertos del helper se renombraron y se anotaron con
`X_INTERFACE_IGNORE = "true"`, mecanismo documentado por
[AMD UG994](https://docs.amd.com/r/2022.1-English/ug994-vivado-ip-subsystems/List-of-Supported-X_-Attributes?contentId=jpcM0W5nwj5Rmx~pVIALuA).
El `system.bd` regenerado ya contiene sólo ocho puertos escalares/vectoriales
en cada una de las instancias `tone_capture_0`, `gaussian_capture_0` y
`prn_capture_0`, sin interfaces inferidas.

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
| BRAM tono | `0x47010000` | `0x87010000` | 4 KiB |
| GPIO tono | `0x47020000` | `0x87020000` | 4 KiB |
| `jam_gaussian_0` | `0x47030000` | `0x87030000` | 64 KiB |
| BRAM ruido | `0x47040000` | `0x87040000` | 4 KiB |
| GPIO ruido | `0x47050000` | `0x87050000` | 4 KiB |
| `gnss_prn_0` (incluye RAM de chips) | `0x47060000` | `0x87060000` | 64 KiB |
| BRAM PRN | `0x47070000` | `0x87070000` | 4 KiB |
| GPIO PRN | `0x47080000` | `0x87080000` | 4 KiB |
| `fp_arraygen_0` | `0x47090000` | `0x87090000` | 64 KiB |
| AD9361 0 | `0x79020000` | `0x99020000` | 64 KiB |
| AD9361 1 | `0x79040000` | `0x99040000` | 64 KiB |
| DMA ADC | `0x7C400000` | `0x9C400000` | 4 KiB |
| DMA DAC | `0x7C420000` | `0x9C420000` | 4 KiB |

Los rangos no se solapan. IRQ existentes: DAC DMA `ps-12`, ADC DMA `ps-13`;
nuevas IRQ tono `ps-14` y ruido/PRN compartida `ps-15`.

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
| `zcu102/Makefile` | Declara `stream_capture.v` y selecciona validación BD por defecto o build completo con `GNSS_FULL_BUILD=1`. |
| `zcu102/system_project.tcl` | Omite síntesis salvo que `GNSS_FULL_BUILD=1`; conserva el ajuste de retardos para el build completo. |
| `zcu102/stream_capture.v` | Capturador compartido por tono y ruido; cuenta transferencias, escribe BRAM y publica estado. |
| `zcu102/tb_stream_capture.sv` | Test focalizado de orden, longitud, contrapresión, errores y rearme. |
| `zcu102/check_tone_package.tcl` | Comprueba VLNV y puertos en XCZU9EG sin síntesis. |
| `zcu102/check_gaussian_package.tcl` | Comprueba VLNV y puertos del ruido en XCZU9EG sin síntesis. |
| `zcu102/check_prn_package.tcl` | Comprueba VLNV y puertos del PRN en XCZU9EG sin síntesis. |
| `zcu102/check_arraygen_package.tcl` | Comprueba VLNV e interfaces de ArrayGen en XCZU9EG sin síntesis. |
| `zcu102/capture_tone.py` | Procedimiento de control y lectura Linux; `Window` admite tamaño de mapa para acceder a la RAM interna PRN; pendiente de prueba en placa. |
| `zcu102/capture_gaussian.py` | Control y lectura Linux del ruido; pendiente de prueba en placa. |
| `zcu102/capture_prn.py` | Carga chips por AXI-Lite, configura el PRN y guarda la captura desde Linux; pendiente de prueba en placa. |

Comandos de esta etapa desde la raíz:

```bash
make -n gnss_arraysim.zcu102
cd /tmp
/tools/Xilinx/Vivado/2022.2/bin/vivado -mode batch \
  -source /home/clara/fpga/git/ad_hdl_custom/projects/gnss_arraysim/zcu102/check_tone_package.tcl
/tools/Xilinx/Vivado/2022.2/bin/vivado -mode batch \
  -source /home/clara/fpga/git/ad_hdl_custom/projects/gnss_arraysim/zcu102/check_gaussian_package.tcl
/tools/Xilinx/Vivado/2022.2/bin/vivado -mode batch \
  -source /home/clara/fpga/git/ad_hdl_custom/projects/gnss_arraysim/zcu102/check_prn_package.tcl
/tools/Xilinx/Vivado/2022.2/bin/vivado -mode batch \
  -source /home/clara/fpga/git/ad_hdl_custom/projects/gnss_arraysim/zcu102/check_arraygen_package.tcl
cd /home/clara/fpga/git/ad_hdl_custom
PATH="/tools/Xilinx/Vivado/2022.2/bin:$PATH" make gnss_arraysim.zcu102
cd /tmp
/tools/Xilinx/Vivado/2022.2/bin/xvlog -sv \
  /home/clara/fpga/git/ad_hdl_custom/projects/gnss_arraysim/zcu102/stream_capture.v \
  /home/clara/fpga/git/ad_hdl_custom/projects/gnss_arraysim/zcu102/tb_stream_capture.sv
/tools/Xilinx/Vivado/2022.2/bin/xelab tb_stream_capture -s tb_stream_capture_sim
/tools/Xilinx/Vivado/2022.2/bin/xsim tb_stream_capture_sim -runall
```

`make -n` terminó con código 0 en esta etapa. La comprobación propia del
paquete gaussiano terminó con código 0 y mostró el VLNV y puertos esperados;
la prueba del paquete de tono corresponde a la etapa anterior. La simulación XSim del
capturador pasó (1024 palabras, orden, pausa, backpressure, longitudes inválidas
y rearme). La validación de `system.bd` en Vivado 2022.2 terminó con código 0,
sin síntesis, en `gnss_bd_validation/`, separado del build completo. Emitió
una advertencia de ancho al unir `bram_addr_a` de 12 bits con `addra` de 32:
el wrapper generado extiende con ceros los bits superiores. La memoria generada
usa puertos de 32 bits y un segmento AXI de 4 KiB. La advertencia crítica sobre
el Tcl Store de usuario es de permisos del entorno; Vivado usó el almacén de
instalación. Siguen las advertencias de la base FMCOMMS5. Quedan pendientes
síntesis, implementación, timing, bitstream y prueba Linux/placa.

El 2026-11-11 se redujeron las dos señales de habilitación BRAM idénticas a
`bram_write` y se quitó el dato BRAM que sólo atravesaba el helper. XSim,
`make -n` y la validación del Block Design pasaron de nuevo.
El wrapper generado conecta `tone_out_TDATA` directamente a `dinb`,
`bram_write` a `enb` y replica su valor en los cuatro bits de `web`.
El ancho de la dirección BRAM sigue siendo 32 bits porque
Vivado marca como crítica la conexión directa de 12 bits a `addrb[31:0]`.

El 2026-11-11 se corrigió la inferencia automática de interfaces del módulo
RTL con `X_INTERFACE_IGNORE` en `tone_valid`, `tone_ready`,
`capture_write_en` y `capture_write_addr`. La simulación XSim y
`make gnss_arraysim.zcu102` pasaron; el nuevo Block Design validó y el wrapper
mantiene las conexiones de datos, dirección y escritura. Se conservó el modo
predeterminado sin síntesis.

La dirección BRAM sigue el formato de byte del controlador: para 32 bits y
4 KiB, AMD documenta que el índice de 1024 palabras ocupa `BRAM_Addr[11:2]`
([PG078, tabla de direcciones BRAM](https://docs.amd.com/api/khub/documents/B6Uh5gbO0aGleXW3kIcXTA/content)).

El usuario informó que, tras resolver la licencia, `make gnss_arraysim.zcu102`
terminó en `OK` con `ADI_MAX_OOC_JOBS=1` **antes** de esta integración digital.
No hemos repetido ese build completo con los cambios actuales.

### Ruido gaussiano: paquete, control y lectura

La IP que carga Vivado está en `library/gnss_arraysim/jam_gaussian_loop/` y
tiene VLNV `xilinx.com:hls:jam_gaussian_loop:1.0`. Se inspeccionaron su
`component.xml`, RTL empaquetado y
`drivers/jam_gaussian_loop_v1_0/src/xjam_gaussian_loop_hw.h`. Ofrece
`s_axi_control`, `ap_clk`, `ap_rst_n`, `interrupt` y AXI4-Stream `noise_out`
con `TDATA[31:0]`, `TVALID` y `TREADY`, sin `TLAST`. El RTL empaqueta
`{Q[15:0], I[15:0]}`; cada componente tiene signo Q2.14. El regslice HLS
propaga `TREADY` y el pipeline se bloquea con contrapresión. El capturador
termina por conteo de transferencias aceptadas.

Su ZIP original es
`gnss_arraysim_hdl/ip_cores/jam_gaussian_gen/ip_packages/jam_gaussian_ip.zip`,
SHA-256 `f1a8f3abf8176e0467bcc02e57a4926bbb8cf8780c637694b840f7e5e024d83d`.
El RTL principal importado coincide byte a byte con el ZIP. Las fuentes HLS
externas no tenían un diff local en `src/` durante esta inspección (HEAD
`ceee7f83f3c973e21c9bb5d50df1cd2ee5c070a2`); los productos HLS y el ZIP
eran archivos sin seguimiento. Sus metadatos indican HLS 2022.2 para
`xczu7eg-ffvc1156-2-e`. `check_gaussian_package.tcl` instanció el VLNV en
XCZU9EG y el Block Design completo validó. Siguen pendientes la síntesis,
timing y prueba física en ZCU102. No se modificó ni reexportó el paquete.

Registros relativos a `0x87030000`: control HLS `0x00` (`ap_start` bit 0,
`ap_done` bit 1 clear-on-read, `ap_idle` bit 2, `ap_ready` bit 3,
`auto_restart` bit 7), GIE `0x04`, IER `0x08`, ISR `0x0c`, `n_samples` `0x10`
(32 bits), `amplitude` `0x18` (16 bits con signo Q2.14) y `seed` `0x20` (32
bits). Una semilla no nula reinicia los dos LFSR; `seed=0` continúa los estados
persistentes. El código HLS transforma los uniformes con Box–Muller y escala
I/Q por `amplitude / sqrt(2)`. La misma longitud se programa en HLS y en el
capturador (1–1024); `auto_restart=0`.

El GPIO de ruido está en `0x87050000`: canal 1 DATA `0x00`, TRI `0x04`;
canal 2 DATA `0x08`, TRI `0x0c`. Tiene exactamente el mismo formato de control
y estado que el GPIO de tono descrito arriba. Orden: comprobar `ap_idle`,
configurar longitud, amplitud y semilla; armar el capturador con ARM 0→1→0;
verificar BUSY; escribir `ap_start=1`; esperar DONE y comprobar contador y
errores; leer `0x87040000 + 4 × índice`; rearmar sólo tras BUSY=0. Los datos
se guardan en una BRAM independiente de 1024 palabras. La IRQ del generador
está en `ps-15`; el software de ejemplo consulta el estado GPIO.

```bash
sudo python3 projects/gnss_arraysim/zcu102/capture_gaussian.py \
  --samples 1024 --amplitude-raw 4096 --seed 0x12345678 \
  --output gaussian_capture.csv
```

`capture_gaussian.py` usa `/dev/mem` y produce CSV de palabras, I/Q enteros y
Q2.14; imprime media I/Q, RMS complejo y correlación compleja de retardo 1.
Hace falta un bitstream nuevo con esta integración, Linux con acceso al mapa
PL y permisos de `/dev/mem` (o un driver/UIO futuro). No se ha ejecutado en
placa. Para una comprobación independiente, repetir dos capturas con la misma
semilla no nula y comparar sus 1024 palabras; calcular media, potencia,
correlación y distribución desde el CSV con otro programa. Como referencia
teórica, con amplitud pequeña que evite desbordamiento, I y Q deberían tener
media próxima a cero, desviación típica próxima a `|amplitude|/sqrt(2)` y RMS
complejo próximo a `|amplitude|`. Son pruebas estadísticas, no garantías de una
ráfaga de 1024 muestras. El repositorio externo documenta fallos previos en
pruebas de correlación del generador gaussiano; la calidad estadística no queda
validada por la integración del Block Design.

Comprobaciones propias: `check_gaussian_package.tcl`, XSim del capturador
compartido, `make -n gnss_arraysim.zcu102` y `make gnss_arraysim.zcu102` con
Vivado 2022.2 terminaron con código 0. El último generó el `.xpr`, validó
`system.bd` y registró `Skipping synthesis`. Las dos instancias de
`stream_capture` muestran sólo ocho puertos RTL y cero interfaces inferidas.
El mapa generado contiene tono `0x87000000/0x87010000/0x87020000` y ruido
`0x87030000/0x87040000/0x87050000`, sin solapamientos, y conserva los dos
DMA de FMCOMMS5. La advertencia de ancho 12→32 bits en el puerto A de cada
BRAM extiende los bits superiores con cero en el wrapper generado. La
advertencia del Tcl Store local procede de permisos del entorno. No se han
ejecutado síntesis, implementación, bitstream ni prueba en placa de esta
revisión.

### PRN: paquete, memoria de chips y captura

El paquete realmente cargado está en `library/gnss_arraysim/gnss_prn_loop/`,
VLNV `xilinx.com:hls:gnss_prn_loop:1.0`. Se revisaron `component.xml`, RTL
empaquetado y `drivers/gnss_prn_loop_v1_0/src/xgnss_prn_loop_hw.h`.
Tiene AXI4-Lite `s_axi_control`, `ap_clk`, `ap_rst_n`, `interrupt` y
AXI4-Stream `prn_out` (`TDATA[31:0]`, `TVALID`, `TREADY`), sin `TLAST`.
El RTL empaqueta `{Q[15:0], I[15:0]}` Q2.14 con signo. El regslice HLS
propaga backpressure; `stream_capture` sólo escribe con `TVALID && TREADY`
y termina por conteo explícito de hasta 1024 muestras.

ZIP de origen: `gnss_arraysim_hdl/ip_cores/gnss_prn_gen/ip_packages/gnss_prn_ip.zip`,
SHA-256 `67855d77884d6149f170ead763263a14202f57a843097467321d7fc0914d6bcf`.
El RTL principal importado y el del ZIP tienen el mismo SHA-256:
`b3657128fa6eea603db0e98f2219bd90c4a808fa72321c7c38506663500a1291`.
Las fuentes HLS `src/` no presentaban cambios locales al inspeccionarlas;
el ZIP era un archivo sin seguimiento en el repositorio externo. El RTL fue
exportado con Vitis HLS 2022.2 para `xczu7eg-ffvc1156-2-e`.
`check_prn_package.tcl` instanció el VLNV en `xczu9eg-ffvb1156-2-e` (ZCU102).
Esto confirma compatibilidad de catálogo e IPI, pero no síntesis ni timing
para la nueva parte. No se modificó ni reexportó el paquete.

Registros relativos a `0x87060000`:

| Offset | Registro | Formato |
| --- | --- | --- |
| `0x00` | HLS AP_CTRL | `ap_start` bit 0, `ap_done` bit 1 clear-on-read, `ap_idle` bit 2, `ap_ready` bit 3, `auto_restart` bit 7 |
| `0x04/0x08/0x0c` | GIE/IER/ISR | Interrupciones HLS |
| `0x10` | `n_samples` | `uint32`, igual que longitud del capturador |
| `0x18` | `prn_length` | `uint32`, 1–4096 chips cargados |
| `0x20` | `sample_step` | IEEE-754 `float32`, chips por muestra |
| `0x28` | `prn_phase_init` | IEEE-754 `float32`, fase de código inicial en chips |
| `0x30` | `doppler_freq` | IEEE-754 `float32`, Hz |
| `0x38` | `carrier_phase_init` | IEEE-754 `float32`, radianes |
| `0x40` | `fs` | IEEE-754 `float32`, Hz del cálculo HLS |
| `0x2000–0x3fff` | `prn_mem` | 4096 chips de 16 bits; dos por palabra de 32 bits |

Cada chip contiene I y Q de 8 bits con signo. El RTL lee I de bits 7:0 y Q
de 15:8; los chips par/impar ocupan bits 15:0/31:16 de una palabra AXI.
El PS carga esta RAM interna antes de arrancar; es independiente de la BRAM
de captura. Los registros de fase inicial sólo se aplican en la primera
ejecución tras reset PL; el estado de código y portadora persiste entre
llamadas. `fs` es un parámetro del algoritmo, no cambia el reloj físico
`sys_cpu_clk` de 100 MHz.

El GPIO PRN `0x87080000` tiene los mismos offsets y bits de armado, pausa y
estado que los GPIO de tono y ruido. La BRAM de lectura está en `0x87070000`
con 1024 palabras de 32 bits. Orden: comprobar `ap_idle=1`, cargar chips y
parámetros, armar 0→1→0 y verificar BUSY, escribir `ap_start=1`, esperar DONE
y contador exacto, leer BRAM y rearmar sólo cuando BUSY=0. `auto_restart=0`.
La IRQ PRN se combina con la gaussiana en `ps-15`; si se usan interrupciones,
el software debe consultar los ISR de ambas IP para identificar la fuente.
El script de ejemplo consulta el GPIO. Si HLS produce más muestras que las
capturadas, puede bloquearse en `TREADY=0` y requerir reset PL.

`capture_prn.py` recibe un CSV sin cabecera con dos enteros I,Q de −128 a 127
por fila. El usuario proporciona el código PRN; el ejemplo no presupone una
secuencia GNSS concreta. Con `chips.csv` preparado:

```bash
sudo python3 projects/gnss_arraysim/zcu102/capture_prn.py \
  --chips-csv chips.csv --samples 1024 --sample-step 0.5 \
  --doppler-hz 1000 --fs-hz 100000000 --output prn_capture.csv
```

El CSV resultante contiene palabra hexadecimal, I/Q enteros y Q2.14.
Hace falta bitstream nuevo, mapa PL accesible y permisos de `/dev/mem` o un
driver/UIO equivalente. No se ha probado en placa. Para comparar contra una
referencia independiente, cargar una secuencia corta conocida, usar fases
iniciales y Doppler cero tras reset PL, y contrastar el chip
`floor(n × sample_step) mod prn_length` con la muestra `chip/127` cuantizada
a Q2.14. Con Doppler distinto de cero, comparar contra la rotación
`exp(j 2π n doppler_freq/fs)` y comprobar frecuencia y continuidad de fase.
La aritmética `float32` y la cuantización HLS pueden diferir en LSB; la
tolerancia experimental y la prueba física siguen pendientes.

Comprobaciones propias para PRN: `check_prn_package.tcl` terminó con código 0
y mostró el VLNV y los puertos esperados para XCZU9EG; `make -n` y
`PATH=/tools/Xilinx/Vivado/2022.2/bin:$PATH make gnss_arraysim.zcu102`
terminaron con código 0. El último generó el `.xpr`, validó `system.bd` y
registró `Skipping synthesis`. El BD contiene los tres capturadores con ocho
puertos RTL cada uno y sin interfaces inferidas. El mapa PS generado muestra
PRN `0x87060000`, BRAM `0x87070000`, GPIO `0x87080000`, sin solapamientos y
conservando los dos DMA FMCOMMS5. La OR de interrupciones llega a
`sys_concat_intc_1/In7` (`ps-15`). Las advertencias de ancho del puerto A de
las tres BRAM producen extensión a cero de los bits superiores en el wrapper.
Quedan pendientes síntesis, implementación, bitstream, timing, prueba Linux y
comprobación de calidad de la secuencia PRN.
El script pasó `--help`; una prueba Python comprobó lectura de CSV, empaquetado
de chips pares/impares (también longitud impar) y codificación IEEE-754. El
script de tono siguió pasando `--help` tras ampliar `Window` a 16 KiB para
el mapa de control PRN; su valor predeterminado sigue siendo 4 KiB.

### ArrayGen: cuarta IP instanciada

Se ha instanciado `fp_arraygen_0` desde el paquete importado
`library/gnss_arraysim/fp_arraygen_loop/`, VLNV
`CTTC:array:fp_arraygen_loop:1.0`. Se revisaron `component.xml`, RTL principal,
cabecera `drivers/fp_arraygen_loop_v1_0/src/xfp_arraygen_loop_hw.h` y fuente
HLS externa. El ZIP de origen es
`gnss_arraysim_hdl/ip_cores/fp_arraygen/ip_packages/zynqMP_arraygen_four_ip.zip`,
SHA-256 `5284fb400d1065bfded1f4140790522fe8da81229eddf72952492a39e9e9a2df`.
El RTL principal del ZIP coincide byte a byte con el importado (SHA-256
`58c21ec8115dc25645bd6451233108fcbb7ca80ad31270cacec592d75bc912a6`).
El ZIP no está versionado en el repositorio externo; las fuentes de `src/`
no tenían diff local al inspeccionarlas. **El nombre del ZIP no identifica la
parte usada para exportar:** el RTL HLS y el script externo indican
`xczu7eg-ffvc1156-2-e`, mientras este proyecto usa
`xczu9eg-ffvb1156-2-e`. El paquete no se modificó ni reexportó.

ArrayGen dispone de `s_axi_control`, reloj y reset, control por pines
`ap_ctrl_chain`, entrada `baseband_in` `ap_fifo` de 24 bits (I/Q de 12),
entrada opcional `w_realtime` `ap_fifo` de 256 bits y salida `arraygen_out`
`ap_fifo` de 128 bits (cuatro antenas × I/Q de 16 bits). No ofrece AXI4-Stream
ni interrupción. Su AXI-Lite **no** incluye `ap_start`: sólo configura
`mode` en offset `0x10` (8 bits), `output_scale` en `0x18` (float32) y pesos
complejos float32 `w1` en `0x20/0x24`, `w2` en `0x2c/0x30`, `w3` en
`0x38/0x3c`, `w4` en `0x44/0x48`. Las direcciones son relativas a
`0x87090000`. La fuente HLS usa los pesos AXI-Lite cuando `mode=0`; en otros
modos puede leer `w_realtime` si hay datos.

El BD conecta `ap_clk` a `sys_cpu_clk` (100 MHz), `ap_rst_n` a
`sys_cpu_resetn` y AXI-Lite al PS. Para mantener intactas las tres capturas y
ambos DMA, el núcleo queda estacionado: `ap_start=0`, `ap_continue=1`,
`baseband_in_empty_n=0`, `w_realtime_empty_n=0`, sus datos de entrada a cero y
`arraygen_out_full_n=0`. No se solicitan lecturas FIFO ni se aceptan
escrituras de salida. Los pines de salida de ArrayGen quedan sin consumidor
en este hito; no se declara procesamiento funcional ni presencia garantizada
de lógica útil tras optimización de síntesis. El PS puede leer/escribir los
registros AXI-Lite, pero eso no inicia el cálculo.

Para activar un camino funcional faltan: selección de fuente entre PRN, tono,
ruido u otra entrada; adaptación de 32 bits Q2.14 a 24 bits I/Q de 12 con una
regla de redondeo/saturación; puente `AXI4-Stream` → `ap_fifo` con
contrapresión; política para `w_realtime` y `ap_ctrl_chain`; consumidor o
capturador de 128 bits; y, si se desea RF, conexión al FIFO DAC FMCOMMS5
preservando el DMA TX. También deben comprobarse síntesis, timing y pruebas
funcionales con ZCU102. Esas decisiones no se deducen de la mera instancia.

Comprobación propia del 2026-11-15: `check_arraygen_package.tcl` registró el
VLNV y sus cinco interfaces en un proyecto en memoria para XCZU9EG; `make -n`
terminó con código 0. El `make` predeterminado también terminó con código 0:
creó `gnss_bd_validation/gnss_arraysim_zcu102.xpr`, validó `system.bd` y
registró `Skipping synthesis`. En el BD generado ArrayGen figura como
`CTTC:array:fp_arraygen_loop:1.0`, su segmento AXI PS es `0x87090000/64K`,
las entradas de parking están conectadas a constantes y permanecen los tres
capturadores y ambos DMA. No ejecutó síntesis, implementación ni bitstream.
La advertencia de permisos del Tcl Store local es ambiental; subsisten las
advertencias previas de ancho 12→32 de los puertos BRAM A.

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

## Comandos y registro de resultados históricos

Comandos ejecutados durante la creación del esqueleto (2026-11-07), antes de
añadir `GNSS_FULL_BUILD`. Para las compilaciones actuales, usar los comandos
de la sección «Compilación: validación por defecto».

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
