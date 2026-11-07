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

La placa objetivo de la integración GNSS es ZCU102. Las copias de ZC702 y ZC706
se mantienen como referencia y sus nombres de proyecto se adaptaron para que
el árbol completo sea coherente; la integración de las IP GNSS se hará sólo en
`zcu102/`.

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
make gnss_arraysim.zcu102
```

- `make -n gnss_arraysim.zcu102`: pasó y resolvió el proyecto y sus nueve
  dependencias ADI. El primer intento detectó que el bloque común aún no se
  había renombrado; se corrigieron el nombre y sus tres referencias y se repitió
  con resultado correcto.
- Compilación Vivado y validación de hardware: pendientes de ejecutar/registrar.

El plan y el inventario de las IP externas están en
[`../../docs/gnss_arraysim_zcu102_plan.md`](../../docs/gnss_arraysim_zcu102_plan.md).
