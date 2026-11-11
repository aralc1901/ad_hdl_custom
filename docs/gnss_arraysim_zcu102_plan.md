# Plan: proyecto GNSS ArraySim para FMCOMMS5 y ZCU102

Fecha inicial: 2026-11-07. Actualización: 2026-11-10. Estado: esqueleto y
catálogo completados; primera captura digital de tono implementada y validada
en Block Design y simulación del capturador. Síntesis e implementación de esta
revisión pendientes.

> Las secciones de inventario y decisiones pendientes conservan el contexto
> histórico. Para la etapa actual se acordó únicamente `jam_tone_loop` →
> captura digital finita en BRAM, manteniendo FMCOMMS5 y ambos DMA. La
> arquitectura, mapa, registros y uso desde Linux están en
> [`../projects/gnss_arraysim/README.md`](../projects/gnss_arraysim/README.md).

## Objetivo y alcance

Crear un proyecto independiente bajo `projects/gnss_arraysim/zcu102/`, basado en
`projects/fmcomms5/zcu102/`. Mantener la instanciación de los dos front-ends
AD9361 y sus conexiones físicas en la ZCU102; integrar las IP GNSS disponibles
en el repositorio externo y documentar la arquitectura, los cambios y las
pruebas. No publicar cambios remotos: los commits serán locales.

El nombre `gnss_arraysim` es provisional hasta confirmar el nombre deseado.

## Reconocimiento realizado

- El árbol de trabajo `ad_hdl_custom` estaba limpio, en la rama `hdl_2022_r2`,
  commit `ae6e248f2` al comenzar este análisis.
- El `Makefile` raíz descubre proyectos mediante `projects/*/Makefile` y ofrece
  la meta `make <proyecto>.<placa>`. Un nuevo proyecto con la estructura habitual
  no requiere modificar ese archivo autogenerado.
- El proyecto de referencia separa el diseño del bloque común
  (`projects/fmcomms5/common/fmcomms5_bd.tcl`) y el específico de ZCU102
  (`projects/fmcomms5/zcu102/{system_bd.tcl,system_project.tcl,system_top.v,system_constr.xdc,Makefile}`).
  Los dos `axi_ad9361` comparten reloj de interfaz y sincronización DAC; el
  segundo tiene `USE_SSI_CLK 0`. Hay ocho canales de 16 bits en cada dirección,
  FIFO, `util_cpack2`/`util_upack2` y dos `axi_dmac` con 128 bits en el lado PL.
- El bloque base de ZCU102 proporciona PS, GPIO, SPI, relojes de 100/250/500 MHz,
  resets, SYSID, interrupciones y acceso a memoria. Las restricciones del
  proyecto FMCOMMS5 asignan ambas interfaces FMC y el reloj de referencia.
- El flujo del repositorio pide Vivado 2022.2 (`scripts/adi_env.tcl`). No se
  encontró `vivado` en el PATH durante el reconocimiento.
- La ruta externa estaba oculta por el sandbox de esta sesión, pero se pudo leer
  en WSL con acceso fuera del sandbox. El inventario figura a continuación.

## Etapas propuestas

1. **Inventario de las IP GNSS.** Localizar el repositorio externo, anotar su
   revisión y enumerar módulos sintetizables, ficheros de empaquetado, puertos,
   parámetros, licencias, pruebas y dependencias. Dibujar el flujo de datos
   requerido para RX y TX y decidir qué IP se conectan realmente, en qué orden
   y con qué configuración. Resultado: tabla de integración y mapa de relojes,
   resets, anchos y protocolos en este documento.
2. **Esqueleto reproducible.** Crear `projects/gnss_arraysim/` con Makefiles y
   ficheros Tcl/Verilog/XDC derivados de FMCOMMS5. Conservar los dos front-ends,
   pinout, sincronización y control SPI/GPIO. Cambiar nombres de proyecto y
   referencias locales, sin incluir artefactos generados de Vivado. Comprobar
   que `make -n gnss_arraysim.zcu102` resuelve la nueva meta.
3. **Incorporación de IP.** Elegir, según el inventario, si las IP se incorporan
   como fuentes propias bajo `library/`, como dependencias versionadas o por
   otra vía reproducible. Registrar procedencia y licencia. Añadir empaquetado
   Vivado y `LIB_DEPS`, instanciar las IP en el Block Design y conectar el flujo
   de muestras. Resolver explícitamente adaptación de 8 canales/16 bits,
   `valid`/`ready` o señales FIFO, dominios de reloj, CDC y reset. Mantener los
   mapas AXI y las interrupciones sin solapamientos.
4. **Verificación.** Revisar elaboración y validación del Block Design; ejecutar
   síntesis/implementación y generar XSA si Vivado 2022.2 está disponible.
   Comprobar warnings de puertos sin conectar, CDC, timing y uso de recursos.
   Añadir una prueba funcional del camino de datos pertinente a las IP GNSS y,
   si hay hardware, comprobar sincronización y transferencia DMA en ZCU102.
   Registrar comandos y resultados reales; no declarar validado lo no ejecutado.
5. **Documentación y entrega.** Completar este Markdown con decisiones,
   diagrama de datos, inventario final, direcciones AXI, parámetros, comandos de
   compilación, resultados y limitaciones. Revisar que Git sólo contenga fuentes
   y documentación.

## Commits locales previstos

Los límites se ajustarán a lo que revele el inventario:

1. `docs: plan GNSS ArraySim ZCU102` — este plan y el reconocimiento inicial.
2. `projects: add GNSS ArraySim ZCU102 baseline` — realizado (`2df3d9613`):
   copia de todo el contenido fuente versionado de FMCOMMS5 y adaptación de
   nombres de proyecto en las tres placas.
3. `library: add GNSS ArraySim IP dependencies` — realizado (`50b41e800`):
   paquetes extraídos en `library/gnss_arraysim`, registro en catálogo y
   verificación. Las IP todavía no se conectan al flujo de muestras.
4. `projects: add GNSS tone digital capture on ZCU102` — primera prueba de
   `jam_tone_loop` con BRAM, simulación y documentación; sin ArrayGen ni RF.
5. Integración de ArrayGen, ruta RF y validación física — etapas posteriores.

No se harán `git push`.

## Dependencias antes de implementar

- Acordar qué generadores deben usarse y cómo se combinan con ArrayGen, ADC y
  DMA. La existencia de un paquete no define por sí sola su conexión funcional.
- Decidir si el DMA TX existente tendrá un selector para conservar reproducción
  desde memoria, o si ArrayGen será la única fuente TX.
- Antes de importar fuentes externas, confirmar licencia y método de versionado.
- Vivado y Vitis HLS 2022.2 están instalados en `/tools/Xilinx/`, aunque no estaban
  en el PATH del primer shell. Para pruebas físicas hará falta ZCU102 con FMCOMMS5.

## Inventario de IP GNSS (etapa 1)

Fuente: [`gnss_arraysim_hdl`](../../gnss_arraysim_hdl/README.md), rama
`feature/jam_gen_v2`, commit `2cc4dc49a1589267d1ce348c6c2fcc0b9d228b4f`.
El árbol externo contiene 79 cambios locales al inventariarlo, principalmente
productos generados de `jam_tone_gen`, y un proyecto Vivado sin seguimiento Git.
Se inspeccionó en solo lectura; no se alteraron esos cambios. Las observaciones
de paquetes se refieren a los ZIP presentes en disco, cuyo contenido puede no
corresponder exactamente al commit indicado.

| Núcleo | Fuente y función | Interfaz exportada | Paquete disponible |
| --- | --- | --- | --- |
| `gnss_prn_loop` | [`gnss_prn_top.cc`](../../gnss_arraysim_hdl/ip_cores/gnss_prn_gen/src/gnss_prn_top.cc): genera muestras desde una memoria de hasta 4096 chips I/Q de 8 bits, con paso de código y Doppler configurables | AXI4-Lite de control y memoria PRN; AXI4-Stream `prn_out`, `TDATA[31:0]` (I/Q Q2.14 de 16 bits) | `gnss_prn_ip.zip`, VLNV `xilinx.com:hls:gnss_prn_loop:1.0` |
| `jam_tone_loop` | [`jam_tone_top.cc`](../../gnss_arraysim_hdl/ip_cores/jam_tone_gen/src/jam_tone_top.cc): tono complejo; número de muestras, amplitud y paso de fase | AXI4-Lite; AXI4-Stream `tone_out`, `TDATA[31:0]` I/Q Q2.14 | `jam_tone_ip.zip`, VLNV `xilinx.com:hls:jam_tone_loop:1.0`; ZIP modificado localmente |
| `jam_gaussian_loop` | [`jam_gaussian_top.cc`](../../gnss_arraysim_hdl/ip_cores/jam_gaussian_gen/src/jam_gaussian_top.cc): ruido complejo; número de muestras, amplitud y semilla | AXI4-Lite; AXI4-Stream `noise_out`, `TDATA[31:0]` I/Q Q2.14 | `jam_gaussian_ip.zip`, VLNV `xilinx.com:hls:jam_gaussian_loop:1.0` |
| `fp_arraygen_loop` | [`fp_arraygen_top.cc`](../../gnss_arraysim_hdl/ip_cores/fp_arraygen/src/fp_arraygen_top.cc): aplica escala y pesos complejos a una entrada para producir cuatro antenas | AXI4-Lite para modo, escala y pesos; control `ap_ctrl_chain`; `ap_fifo` de entrada de 24 bits (I/Q de 12), pesos de 256 bits y salida de 128 bits (4 × I/Q de 16) | `zynqMP_arraygen_four_ip.zip` y variante `full`, ambos VLNV `CTTC:array:fp_arraygen_loop:1.0`; no versionados |

Los tres generadores HLS tienen también `interrupt` y `ap_clk`/`ap_rst_n` en los
paquetes. Sus salidas no incluyen `TLAST`; se deben delimitar ráfagas por
`n_samples` o añadir la señal en un adaptador si el consumidor la requiere.
ArrayGen usa `baseband_in_dout/empty_n/read` y
`arraygen_out_din/full_n/write`, no un puerto AXI4-Stream. El FIFO de pesos puede
quedar inactivo si `mode=0` y se usan pesos AXI4-Lite, tras verificar en Vivado
las condiciones de entrada del puerto. Los otros tres paquetes de ArrayGen
(`Artix7`, `Zynq`, `ZynqMP full`) son variantes; `jam_gen_ip.zip` es un núcleo
legado sin fuente HLS correspondiente en este
árbol. No se deben cargar dos paquetes con el mismo VLNV.

### Compatibilidad con la referencia FMCOMMS5

El proyecto base mueve cuatro muestras complejas de 16 bits por instante: ocho
canales I/Q de 16 bits, 128 bits en total. La salida de ArrayGen coincide con ese
ancho total, pero la correspondencia exacta de bits y el orden de antenas debe
verificarse en simulación o con el `component.xml` y la lógica de empaquetado.
Su entrada de 24 bits requiere conversión explícita desde los generadores de
32 bits y una regla documentada de redondeo/saturación de 16 a 12 bits. Además
se necesita un puente AXI4-Stream a `ap_fifo`, control de contrapresión y un
distribuidor de la salida de 128 bits hacia los ocho puertos de `util_rfifo`.
La compatibilidad de ritmo entre ráfagas HLS y el DAC continuo aún no está
demostrada: se debe definir buffering y comportamiento durante huecos.

Los scripts HLS del PRN, tono y ruido apuntan a
`xczu7eg-ffvc1156-2-e`; ArrayGen ofrece una variante para
`xczu9eg-ffvb1156-2-e`, la parte usada por ZCU102 en la referencia. Antes de
integrar los tres primeros hay que reexportarlos para la parte destino o
comprobar formalmente la compatibilidad en Vivado 2022.2. El proyecto Vivado
no versionado del repositorio externo contiene tono, ArrayGen, un FIFO AXI e
ILA, pero no constituye un esquema completo de FMCOMMS5.

La documentación existente en [`verification/README.md`](../../gnss_arraysim_hdl/ip_cores/verification/README.md)
indica simulación C satisfactoria para tono y PRN, y cuatro fallos estadísticos
en el generador gaussiano por correlación de potencia entre muestras. Es un
resultado documentado en el repositorio externo, no una prueba repetida aquí.
El mismo documento indica que no se hizo cosimulación RTL ni prueba de placa.
No se encontró licencia explícita en la raíz del repositorio externo ni en los
directorios fuente inspeccionados; hay que aclararla antes de copiar IP.

### Topología candidata y decisiones pendientes

Una topología a evaluar es: generador seleccionado → conversión Q2.14 a I/Q de
12 bits → ArrayGen de cuatro antenas → distribuidor de 128 bits → FIFO DAC
FMCOMMS5. El ADC y DMA RX pueden conservarse inicialmente para observación.
Si se desea combinar PRN, tono y ruido, hace falta un sumador con escala,
saturación y reglas de activación; ninguno de esos núcleos realiza esa mezcla.
También está pendiente escoger un selector entre ArrayGen y DMA TX para mantener
la reproducción desde memoria.

Esta topología es una hipótesis de integración, no una conexión aprobada ni
validada. Antes de la etapa 3 hay que precisar las señales que deben coexistir,
los caudales y el uso esperado de ADC/DMA. El inventario evita asignar aún
direcciones AXI o IRQ, pues dependerán de la arquitectura elegida.

## Registro de trabajo

- 2026-11-07: inspeccionados `Makefile`, los ficheros fuente de FMCOMMS5/ZCU102,
  el bloque común ZCU102, el flujo de Make y `adi_env.tcl`; comprobado el estado
  de Git, la ruta externa y la disponibilidad de Vivado. No se ha modificado HDL.
- 2026-11-07: localizada la ruta UNC de WSL fuera del sandbox; leídas revisión,
  estado Git, fuentes HLS, Tcl, documentación, metadatos `component.xml` de los
  ocho ZIP y el Block Design experimental. Registrados anchos, protocolos,
  discrepancia de partes y resultado de verificación documentado. No se ejecutó
  HLS ni se modificó el repositorio externo.
- 2026-11-07: copiados los 20 archivos fuente versionados de `projects/fmcomms5`
  a `projects/gnss_arraysim`, incluido `common/` y las tres placas. No se
  arrastraron 281 MB de salidas locales de Vivado. Se renombraron identificadores
  de proyecto y el bloque común; se verificó con `cmp` que el bloque AD9361,
  `system_top.v` y `system_constr.xdc` de ZCU102 mantienen su contenido. El
  primer `make -n gnss_arraysim.zcu102` detectó una referencia a un nombre de
  fichero aún no adaptado; corregida y repetida, terminó con código 0. Commit
  local: `2df3d9613`.
- 2026-11-07: la compilación base con `PATH=/tools/Xilinx/Vivado/2022.2/bin:$PATH
  make gnss_arraysim.zcu102` creó y validó el Block Design, pero falló en
  síntesis por licencia Vivado `[Common 17-345]` para `Synthesis`/`xczu9eg`.
  No se generó XSA. Los paquetes PRN, tono, ruido y ArrayGen se extrajeron
  completos bajo `library/gnss_arraysim`; hashes y procedencia en su README.
  `check_ip_catalog.tcl` encontró los cuatro VLNV en Vivado 2022.2. La
  validación `ADI_SKIP_SYNTHESIS=1` del proyecto completo terminó con código 0,
  sin ejecutar la síntesis ni el ajuste de retardos que depende de ella. Las IP
  externas todavía no están instanciadas en el camino de muestras. Commit local
  del catálogo y paquetes: `50b41e800`.
- 2026-11-10: el usuario informó que resolvió la licencia y que
  `ADI_MAX_OOC_JOBS=1 make gnss_arraysim.zcu102` terminó en `OK` para el estado
  anterior a la captura. Es un resultado **reportado por el usuario**, no una
  compilación repetida en esta etapa.
- 2026-11-10: se comprobó el estado Git antes de editar. Había diez borrados
  locales del usuario bajo `projects/gnss_arraysim/zc702/` y `zc706/`; se
  conservaron y no pertenecen al commit de esta etapa. Se inspeccionaron el
  `component.xml`, RTL y cabecera de registros del paquete de tono importado,
  y se compararon con las fuentes HLS externas. Se confirmó VLNV
  `xilinx.com:hls:jam_tone_loop:1.0`, AXI4-Lite, `ap_clk`, `ap_rst_n`, IRQ,
  AXI4-Stream de 32 bits sin TLAST, `{Q,I}` Q2.14, parámetros y propagación de
  `TREADY`. El ZIP conserva cambios locales del origen y fue exportado para
  XCZU7EG; no se modificó. Vivado 2022.2 lo instanció en un proyecto en memoria
  para XCZU9EG y validó el Block Design final, sin hacer síntesis/timing.
- 2026-11-10: se añadió un capturador de hasta 1024 muestras aceptadas por
  `TVALID && TREADY`, con estado, rechazo de longitud inválida, pausa y rearme.
  El PS configura el tono y el capturador por AXI4-Lite/GPIO y lee BRAM a
  `0x87010000`. Direcciones finales: tono `0x87000000`, BRAM `0x87010000`,
  GPIO `0x87020000`; IRQ tono `ps-14`. El orden es armar, arrancar, esperar
  DONE, leer y rearmar. El contador sustituye TLAST. FMCOMMS5 y los dos DMA
  permanecen conectados. `make -n` pasó; la simulación XSim focalizada pasó;
  la validación de `system.bd` pasó en `gnss_tone_validation4/`, sin alterar el
  build existente. Hubo una advertencia de conexión de ancho BRAM 12→32 bits
  con extensión de ceros en el wrapper y una advertencia de permisos del Tcl
  Store local. No se han ejecutado síntesis ni implementación con esta revisión,
  ni se ha probado el acceso Linux a la placa.
- 2026-11-11: se simplificó la interfaz BRAM del capturador a una habilitación
  de escritura y dirección. El dato `tone_out_TDATA` pasa directamente a la
  BRAM. La habilitación gobierna `enb` y `web` del
  puerto B; el puerto A sigue bajo control AXI del PS. Se probó una dirección
  de sólo 12 bits, pero Vivado dejó sin conectar los bits superiores del puerto
  BRAM de 32 bits y emitió `[BD 41-2383]`; se restauró el ancho de 32 bits con
  los 20 superiores a cero. La simulación focalizada XSim, `make -n` y la
  validación del Block Design pasaron de nuevo en `gnss_tone_validation7/`.
  El wrapper final conecta `tone_out_TDATA` directamente a `dinb` y
  `bram_write` a `enb` y a los cuatro bits de `web`.
