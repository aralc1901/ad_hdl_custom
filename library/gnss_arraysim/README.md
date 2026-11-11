# Paquetes HLS para GNSS ArraySim

Cuatro IP exportadas desde el repositorio local `gnss_arraysim_hdl`, rama
`feature/jam_gen_v2`, HEAD `2cc4dc49a1589267d1ce348c6c2fcc0b9d228b4f`.
Se extrajeron completos para que el IP Catalog de Vivado pueda leer sus
`component.xml`. No se copiaron las soluciones HLS ni otros resultados de build.

| Directorio | ZIP de origen | SHA-256 del ZIP |
| --- | --- | --- |
| `gnss_prn_loop/` | `ip_cores/gnss_prn_gen/ip_packages/gnss_prn_ip.zip` | `67855d77884d6149f170ead763263a14202f57a843097467321d7fc0914d6bcf` |
| `jam_tone_loop/` | `ip_cores/jam_tone_gen/ip_packages/jam_tone_ip.zip` | `ff772f853e5c03484c249a56bdd75f83c9ce6a309bd6a5806de8180c8735bbe4` |
| `jam_gaussian_loop/` | `ip_cores/jam_gaussian_gen/ip_packages/jam_gaussian_ip.zip` | `f1a8f3abf8176e0467bcc02e57a4926bbb8cf8780c637694b840f7e5e024d83d` |
| `fp_arraygen_loop/` | `ip_cores/fp_arraygen/ip_packages/zynqMP_arraygen_four_ip.zip` | `5284fb400d1065bfded1f4140790522fe8da81229eddf72952492a39e9e9a2df` |

`jam_tone_ip.zip` tenía cambios locales sin commit en el repositorio de origen.
El ZIP de ArrayGen no estaba versionado allí. Los hashes identifican exactamente
el contenido importado y evitan atribuirlo erróneamente sólo al commit HEAD.
No se encontró una licencia explícita en el repositorio de origen; aclarar la
licencia antes de redistribuir estos paquetes.

El paquete ArrayGen seleccionado es para `xczu9eg-ffvb1156-2-e`, la parte ZCU102.
Los scripts de exportación de PRN, tono y ruido apuntan a `xczu7eg`; su
compatibilidad de síntesis para ZCU102 no está demostrada. El paquete gaussiano
tiene fallos estadísticos documentados en el repositorio de origen. Tono y
ruido ya están instanciados para capturas digitales independientes en ZCU102;
PRN y ArrayGen siguen sólo en el catálogo. El paquete gaussiano no se ha
modificado ni reexportado; se validó su instanciación IPI para XCZU9EG, pero
siguen pendientes síntesis, timing y comprobación estadística en placa.
