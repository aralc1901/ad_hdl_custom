# PetaLinux de pruebas GNSS ArraySim: ZCU102 + FMCOMMS5

Este README concentra la documentación de Linux del proyecto. Para el diseño
HDL, registros HLS y comportamiento de los capturadores, consultar el
[README principal](../README.md).

Flujo nuevo e independiente del proyecto `petalinux_gnss_smart`. Requiere
PetaLinux **2022.2**, usuario normal, Git, Python 3 y acceso a Internet para
Yocto. El hardware debe terminar síntesis, implementación y timing antes de
importarlo. No se acepta el XSA de un diseño sólo validado ni un XSA sin `.bit`.

Se genera un proyecto zynqMP desde plantilla y una imagen de laboratorio con
rootfs en **initramfs**: no requiere partición ext4, y los CSV guardados en `/tmp`
se pierden al reiniciar. Incluye Python estándar (sin NumPy), las tres capturas,
SSH/SFTP, kernel ADI, libiio, herramientas IIO, iiod y libad9361-iio. El kernel
conserva la configuración de referencia ADI; no se ha reducido driver a driver.

## Archivos y responsabilidades

| Archivo o directorio | Función |
| --- | --- |
| `scripts/prepare.sh` | Verifica XSA, descarga la revisión fijada de meta-adi, crea el proyecto, importa hardware e instala recetas y capturas. |
| `scripts/build.sh` | Comprueba el hash del XSA importado, ejecuta `petalinux-build` y llama al empaquetado. |
| `scripts/package.sh` | Verifica resultados, genera BOOT.BIN y copia los archivos de arranque a una entrega fechada. |
| `scripts/docker-run.sh` | Ejecuta prepare/build/package o abre una shell en el contenedor 2022.2. |
| `scripts/common.sh` | Define rutas, revisión de meta-adi y comprobaciones del entorno; lo cargan los demás scripts. |
| `scripts/validate_xsa.py` | Comprueba bitstream, dispositivo XCZU9EG y direcciones PS de GNSS, AD9361 y DMA. |
| `scripts/configure_project.py` | Aplica los ajustes gestionados de capas, initramfs, paquetes y revisiones. |
| `scripts/check_artifacts.py` | Inspecciona DTB, configuración final del kernel y coincidencia del bitstream con el XSA. |
| `meta-user/recipes-kernel/` | Fragmento del kernel para ADI/IIO y acceso a `/dev/mem`. |
| `meta-user/recipes-bsp/device-tree/` | Extensión del device tree y aislamiento de los registros de captura. |
| `meta-user/recipes-apps/gnss-capture/` | Receta que instala capturas, chips de ejemplo y `gnss-check`. |
| `work/` | Proyecto generado, fuentes externas y resultados de Yocto; no versionados. |
| `artifacts/` | Entregas fechadas para la SD; no versionadas. |

Los Python de captura tienen una única fuente mantenida en `../zcu102/`.
`prepare.sh` copia sus versiones actuales a la receta del proyecto generado.
No editar esas copias para cambios permanentes.

La receta del device tree incorpora explícitamente `system-user.dtsi` y
`system-conf.dtsi` mediante `SRC_URI` y los instala desde `WORKDIR` al
directorio generado por DTG antes de ejecutar el aislamiento y la integración
ADI. `system-conf.dtsi` fija UART0 a 115200 e initramfs sin raíz en SD; si se
cambia el método de arranque, actualizar también ese archivo. No se debe
suponer que Xilinx genera automáticamente estos dos archivos de usuario.

## Compilación

Desde este directorio, en una terminal nueva con PetaLinux 2022.2:

```bash
cd /home/javier/zynq/hardware/git/clara_ad_hdl_custom/projects/gnss_arraysim/petalinux
source /ruta/a/petalinux/2022.2/settings.sh
./scripts/prepare.sh
./scripts/build.sh
```

`prepare.sh` usa por defecto
`../zcu102/gnss_arraysim_zcu102.sdk/system_top.xsa`. También admite otro XSA:

```bash
./scripts/prepare.sh /ruta/absoluta/system_top.xsa
```

En esta máquina existe el entorno Docker `docker_petalinux2`. Si está instalada
la imagen **2022.2**, se puede usar sin instalar PetaLinux en el host:

```bash
./scripts/docker-run.sh prepare
./scripts/docker-run.sh build
```

El wrapper no construye ni descarga automáticamente esa imagen. Para un nombre
local diferente: `GNSS_PETA_IMAGE=nombre:2022.2 ./scripts/docker-run.sh prepare`.
La imagen debe tener el usuario `petalinux` con permiso de escritura en el
repositorio y cargar `/opt/Xilinx/petalinux/settings.sh` al entrar en bash login,
como hace el Dockerfile local de `docker-petalinux2`. No necesita USB ni modo
privilegiado. El repositorio se monta en su misma ruta absoluta; el XSA debe estar
dentro de él. Para inspeccionar el entorno: `./scripts/docker-run.sh shell`.

Los scripts paran ante un error y guardan el proyecto en
`work/gnss-arraysim-zcu102/`, ignorado por Git. `prepare.sh` se puede repetir:
reimporta hardware, actualiza las capturas y restaura la configuración gestionada.
No borra el proyecto ni la caché. Para actualizar HDL, repetir prepare y build;
no reemplazar sólo el `.bit` de una imagen anterior.

`build.sh` construye la imagen completa y llama a `package.sh`. Este último
comprueba el DTB, los drivers y la identidad del bitstream con el XSA, y genera
BOOT.BIN con FSBL, PMUFW, ATF, U-Boot y FPGA. Se puede repetir por separado:

```bash
./scripts/package.sh
```

Los resultados se copian a `artifacts/<fecha-UTC>/`: `BOOT.BIN`, `image.ub`,
`boot.scr`, `system.dtb` y hashes. Copiar los cuatro archivos a una partición
FAT de una SD, configurar ZCU102 para arranque SD y conectar la consola UART
(115200, 8N1). `image.ub` contiene kernel, device tree e initramfs; el DTB suelto
sirve también para inspección. No se formatea ninguna SD desde estos scripts.

## Pruebas en placa

La capa ADI 2022_R2 establece las credenciales de laboratorio `analog/analog`
y contraseña de root `analog`. Para acceso a `/dev/mem`, iniciar sesión root por
consola o usar `sudo`. El acceso SSH root depende de la configuración de sshd;
usar el usuario `analog` y sudo. Es una imagen de banco de pruebas.

```bash
sudo gnss-check
cd /opt/gnss-arraysim
sudo python3 capture_tone.py --samples 1024 --amplitude-raw 4096 \
  --phase-step 42949673 --sample-rate-hz 100000000 --output /tmp/tone.csv
sudo python3 capture_gaussian.py --samples 1024 --amplitude-raw 4096 \
  --seed 1 --output /tmp/gaussian.csv
sudo python3 capture_prn.py --chips-csv chips-example.csv --samples 1024 \
  --sample-step 0.5 --doppler-hz 0 --fs-hz 100000000 --output /tmp/prn.csv
```

`chips-example.csv` es una secuencia sintética de diagnóstico, no un PRN de
satélite. No indicar `--phase-start 0` en capturas sucesivas de tono salvo tras
reset PL; consultar `--help` y el README HDL para las referencias y continuidad
de fase. Las pruebas son finitas, con máximo 1024 muestras; no arrancan al boot.
`gnss-check` enumera IIO y comprueba Python; no afirma que las capturas físicas
hayan pasado. ArrayGen conserva sólo acceso a sus registros en `0x87090000`;
el HDL lo mantiene estacionado: Linux no añade los adaptadores ni su salida RF.

## ADI, propiedad de registros y futura conexión RF

Se fija meta-adi en `3a5de5cf8f1cf4a0fe39a26cbcc6a292a0e5b654` (2022_R2),
el kernel en `cd7e20c430dc19df7c32610e9d5b494d8f313e07` y libad9361-iio en
`2bf936d6afcb3e6385642b6a769fd74f8ab9b62d`. Otros paquetes siguen las recetas
de esa capa y de PetaLinux; no es un build completamente reproducible/offline.

Se selecciona el device tree oficial
`zynqmp-zcu102-rev10-ad9361-fmcomms5`. Conserva SPI0 con ambos AD9361,
GPIOs de reset/sincronismo/calibración, los bloques ADC/DAC y ambos DMA.
Sus direcciones corresponden al HDL: AD9361 `0x99020000/0x99040000`,
DMA `0x9c400000/0x9c420000`. Los pines reset PS son 130/143 y sync 129,
correspondientes a EMIO 52/65/51. meta-adi combina ese árbol con los nuevos
nodos del XSA y elimina los duplicados de la radio.

Los nodos añadidos entre `0x87000000` y `0x87090000` quedan deshabilitados en
DT para evitar que GPIO/HLS/BRAM drivers reclamen registros o IRQ usados por
los Python. El kernel permite `/dev/mem` sin STRICT_DEVMEM. Esto no deshabilita
su hardware ni los drivers ADI: el acceso a la radio se realiza con IIO, nunca
mediante esos scripts. Las IRQ compartidas de ruido/PRN no se solicitan;
las capturas hacen polling del GPIO.

```bash
iio_info -s
iio_info -u local:
# Desde otro equipo, si iiod está activo y Ethernet tiene dirección IP:
iio_info -u ip:IP_DE_LA_ZCU102
```

Deben aparecer `ad9361-phy`, `ad9361-phy-B` y los dispositivos IIO ADC/DAC.
`libad9361-iio` deja disponible la sincronización multichip para software futuro.
No se arranca TX ni se cambia frecuencia/ganancia automáticamente con estos
scripts. La conexión generadores → ArrayGen → DAC requiere cambios HDL
adicionales; la disponibilidad de IIO no conecta ese camino.

## Estado de validación y diagnóstico

Se han comprobado sintaxis y lógica de preparación localmente, y el mapa del
XSA real. Tras corregir la incorporación de los archivos de usuario del device
tree, se verificaron `device-tree:do_configure`, `do_compile` y `do_deploy`
en el contenedor PetaLinux 2022.2 con el XSA real. El kernel también completó
`do_compile`. **Pendiente: build completo PetaLinux 2022.2 y prueba de arranque/RF
en ZCU102.** Los scripts verifican resultados antes de empaquetar; cualquier
incompatibilidad de DTG o configuración de kernel detiene el empaquetado.

Logs: `work/gnss-arraysim-zcu102/build/build.log` y los `log.do_*` bajo
`build/tmp/work/`. Para cambios manuales: entrar con `docker-run.sh shell`,
hacer `cd work/gnss-arraysim-zcu102` y usar `petalinux-config`,
`petalinux-config -c kernel` o `petalinux-config -c rootfs`. Un nuevo prepare
restaura los ajustes gestionados y las recetas locales.

## Rutas alternativas y actualización

Con ejecución nativa, `GNSS_PETA_WORK` permite colocar el proyecto y meta-adi
en otra ubicación. Usar una ruta absoluta y la misma variable en cada paso:

```bash
export GNSS_PETA_WORK=/ruta/absoluta/gnss-peta-work
./scripts/prepare.sh
./scripts/build.sh
```

El wrapper Docker no propaga esta variable ni monta directorios externos:
usar sus rutas predeterminadas dentro del repositorio. `GNSS_PETA_IMAGE`
selecciona la imagen del wrapper; hay que establecerla en cada invocación
o exportarla en la terminal.

Después de modificar HDL, terminar el build completo y repetir prepare/build.
Después de modificar los Python, recetas o configuración gestionada, también
repetir prepare/build. Para guardar cambios de forma permanente, editar los
archivos de este subdirectorio o los Python fuente, no sólo `work/`.
Cambiar las revisiones fijadas de ADI requiere revisar de nuevo su compatibilidad
con Vivado/PetaLinux 2022.2 y con el device tree de FMCOMMS5.

## Direcciones usadas desde Linux

Estas son direcciones físicas del PS, incluyendo el desplazamiento del mapa
ZynqMP; no sustituirlas por las direcciones de las llamadas Tcl ADI.

| Función | Control HLS | BRAM | GPIO de captura |
| --- | --- | --- | --- |
| Tono | `0x87000000` | `0x87010000` | `0x87020000` |
| Ruido gaussiano | `0x87030000` | `0x87040000` | `0x87050000` |
| PRN | `0x87060000` | `0x87070000` | `0x87080000` |

ArrayGen está en `0x87090000`. El control PRN incluye RAM de chips y requiere
la ventana de 16 KiB que usa `capture_prn.py`. Las BRAM de captura contienen
1024 palabras de 32 bits; cada palabra empaqueta I en bits 15:0 y Q en bits
31:16, ambos enteros con signo Q2.14.

## Recuperar resultados y diagnosticar problemas

Para copiar los CSV a otro equipo, con Ethernet y SSH disponibles, ejecutar
desde ese otro equipo, sustituyendo la IP:

```bash
scp analog@IP_DE_LA_ZCU102:/tmp/tone.csv .
scp analog@IP_DE_LA_ZCU102:/tmp/gaussian.csv .
scp analog@IP_DE_LA_ZCU102:/tmp/prn.csv .
```

Antes de apagar, copiar los resultados que deban conservarse. Para comprobar
la conectividad desde la consola de la placa, usar `ip addr`. Para consultar
los dispositivos IIO sin asumir sus números:

```bash
for name in /sys/bus/iio/devices/iio:device*/name; do
    printf '%s: ' "$name"
    cat "$name"
done
dmesg | grep -Ei 'ad9361|axi|dma|spi|error|fail'
```

| Síntoma | Comprobación o acción |
| --- | --- |
| No se encuentra `petalinux-config` o la versión no es 2022.2 | Cargar el `settings.sh` correcto o entrar en el contenedor 2022.2. |
| Docker no encuentra la imagen | Consultar `docker image ls` y establecer `GNSS_PETA_IMAGE` con su nombre/tag real. El wrapper no instala PetaLinux. |
| Error de permisos dentro de Docker | Comprobar que el usuario `petalinux` del contenedor puede escribir en el repositorio montado. |
| XSA ausente, sin bitstream o con mapa incompatible | Terminar el build HDL completo y pasar el XSA correcto a prepare. No usar el resultado de la validación BD. |
| meta-adi tiene otra revisión o cambios locales | Revisar el checkout en `work/meta-adi`; no se sobrescribe automáticamente un checkout modificado. |
| Error de descarga de Yocto | Consultar el primer `log.do_fetch` que falla y comprobar red, DNS y acceso a las fuentes. |
| Fallo en la verificación del kernel o DTB | Revisar el `.config` final, el DT generado y el XSA. No saltarse el control para empaquetar una combinación incompatible. |
| `FileNotFoundError: .../system-user.dtsi` durante `device-tree:do_configure` | Corregido incorporando ambos `.dtsi` a la receta e instalándolos antes del aislamiento. Repetir prepare/build para aplicar la receta actual; no hace falta borrar el build. |
| Captura rechazada por permisos | Ejecutarla como root o con sudo; comprobar `/dev/mem` y la configuración del kernel. |
| Generador ocupado, timeout o contador incorrecto | Comprobar que se arrancó con el bitstream correspondiente y que longitud HLS y captura coinciden. Un flujo residual puede requerir reset/rearranque de PL. |
| No aparecen los dos PHY AD9361 | Revisar alimentación y montaje FMCOMMS5, SPI, relojes, reset y mensajes de `dmesg`. No basta con que los Python de BRAM funcionen. |
| IIO funciona localmente pero no por red | Comprobar dirección IP, conectividad, servicio `iiod` y acceso al puerto TCP 30431. |

## Referencias

- https://github.com/analogdevicesinc/meta-adi/tree/2022_R2/meta-adi-xilinx
- https://github.com/analogdevicesinc/linux/blob/2022_R2/arch/arm64/boot/dts/xilinx/zynqmp-zcu102-rev10-ad9361-fmcomms5.dts
