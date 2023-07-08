# wafel_setup_mlc

This plugin for [stroopwafel](https://github.com/shinyquagsire23/stroopwafel) allows you to rebuild your mlc. For that it will

- Disables IOSU panic, to keep it from crashing during setup (patch from Gary
- Format MLC (or slcmtp) if they are detected raw (enabled my a patch from quarky)
- Creates a 3GB quota for `sys` on the MLC
- Creates minimal required `usr` directories on MLC
- installs titles from `sd:/wafel_install`
- triggers a factory reset on reboot

## How to use

- copy the wups of all 52 mlc system titles to `sd:/wafel_install`
- put the `wafel_setup_mlc.ipx` together with wafel_core.ipx from stroopwafel in `/wiiu/ios_plugins` on your sdcard
- boot with defuse and watch the serial log
- wait till it says it's done
- power off
- remove `wafel_setup_mlc.ipx` from `/wiiu/ios_plugins`
- boot Wii U, a factory reset start automatically

If you are using the same size media or didn't replace the media the format might not run, because the old wfs is still detected. To force a format see [wafel_destroy_mlc](https://github.com/jan-hofmeier/wafel_destroy_mlc)


## Building

```bash
export STROOPWAFEL_ROOT=/path/too/stroopwafel-repo
make
```

## Replacing the MLC

One way to replace the MLC on your Wii U would be to replace the 8GB / 32GB eMMC with a micro SD card. To make the replacement more convieneint I recommend Voultars [Wii-U NAND-AID - eMMC Recovery and Replacement Interposer](https://gbatemp.net/threads/wii-u-nand-aid-emmc-recovery-and-replacement-interposer-public-test.630798/). Stock IOSU supports up to 64GB MLC. If you want more, you need to to [remove this limitation](https://github.com/jan-hofmeier/wafel_unlimit_mlc).

The other way is to [replace the Disc Drive with a SATA SSD/HDD](https://gbatemp.net/threads/wii-u-internal-storage-upgrades-are-possible.635629/) using a SATA adapter. This option doesn't have size limitation other than the WFS limit of 2TB.


## Thanks

- [GaryOrderNichts](https://github.com/GaryOderNichts) for the IOSU patch to keep IOSU from crashing
- [Ash](https://github.com/ashquarky) for the IOSU patch to enable format
- [V10lator](https://github.com/V10lator) for quota creation and improving title install
- [Lazr1026](https://github.com/Lazr1026) for testing and debugging
