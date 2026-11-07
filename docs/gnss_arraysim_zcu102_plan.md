# Plan: proyecto GNSS ArraySim para FMCOMMS5 y ZCU102

Fecha: 2026-11-07. Estado: planificación; implementación pendiente de acordar el primer paso.

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
- La ruta indicada, `/home/clara/fpga/git/gnss_arraysim_hdl`, no existe en este
  entorno. Por ello aún no se han podido inventariar las IP externas ni fijar
  sus interfaces, licencias, relojes, resets o puntos de inserción.

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
2. `projects: add GNSS ArraySim ZCU102 baseline` — copia adaptada y comprobable
   del diseño FMCOMMS5, sin IP GNSS.
3. `library: add GNSS ArraySim IP dependencies` — fuentes y empaquetado
   necesarios, si procede incorporarlos a este repositorio.
4. `projects: integrate GNSS ArraySim signal chain` — instancias, conexiones y
   direcciones de control.
5. `docs: record GNSS ArraySim validation` — resultados verificables y guía de
   reproducción; se combinará con el anterior si la validación queda completa.

No se harán `git push`.

## Dependencias antes de implementar

- Confirmar o facilitar la ubicación accesible de `gnss_arraysim_hdl`.
- Con el inventario, concretar el comportamiento esperado de las IP: qué
  transformación se aplica a los ocho canales RX/TX y dónde se toma/inyecta la
  señal respecto a los DMA existentes.
- Para validación completa, disponer de Vivado 2022.2 y, para pruebas físicas,
  de la placa ZCU102 con FMCOMMS5.

## Registro de trabajo

- 2026-11-07: inspeccionados `Makefile`, los ficheros fuente de FMCOMMS5/ZCU102,
  el bloque común ZCU102, el flujo de Make y `adi_env.tcl`; comprobado el estado
  de Git, la ruta externa y la disponibilidad de Vivado. No se ha modificado HDL.
