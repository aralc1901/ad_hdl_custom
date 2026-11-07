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
Por ahora están disponibles en el IP Catalog, sin instancias ni conexiones al
camino de muestras. Se conserva el bloque de radio de referencia.

## Direcciones AXI existentes

Proceden de `common/gnss_arraysim_bd.tcl` y del diseño base de ZCU102. No se han
asignado todavía direcciones a las IP GNSS.

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
