# wafel_setup_mlc

This is a plugin for [stroopwafel](https://github.com/shinyquagsire23/stroopwafel) that allows you to rebuild your mlc. For that it will:

- Disable IOSU panic, to keep it from crashing during setup (patch from Gary)
- Format MLC (or SLCCMPT) if they are detected raw (enabled by a patch from quarky)
- Creates the required quotas/folders on the MLC
- Installs titles from `sd:/wafel_install`
- Fixes the region if needed
- Triggers inital setup on next boot

## How to use

- Copy the WUPs of all 52 mlc system titles to `sd:/wafel_install`
- Put the `wafel_setup_mlc.ipx` together with `wafel_core.ipx` from stroopwafel in `/wiiu/ios_plugins` on your SD Card
- Boot with de_Fuse or isfshax and watch the serial log (or check the log on the SD when its done)
- Wait till it says it's done / the LED stops blinking
- Power off
- Remove `wafel_setup_mlc.ipx` from `/wiiu/ios_plugins`
- Boot the Wii U, the initial setup should launch

If you are using the same size media or didn't replace the media the format might not run, because the old WFS is still detected. To force a format, select `Wipe MLC` and `Delete scfm.img` in `Backup and Restore`.

The MLC system titles can be downloaded using the [MLCRestorerDownloader by Xpl0itU](https://github.com/Xpl0itU/MLCRestorerDownloader)


## Building

```bash
export STROOPWAFEL_ROOT=/path/too/stroopwafel-repo
make
```

## Replacing the MLC

One way to replace the MLC on your Wii U would be to replace the 8GB / 32GB eMMC with a micro SD card. To make the replacement more convieneint I recommend Voultars [Wii-U NAND-AID - eMMC Recovery and Replacement Interposer](https://gbatemp.net/threads/wii-u-nand-aid-emmc-recovery-and-replacement-interposer-public-test.630798/). Stock IOSU supports up to 64GB MLC. If you want more, you need to to [remove this limitation](https://github.com/jan-hofmeier/wafel_unlimit_mlc).

The other way is to [replace the Disc Drive with a SATA SSD/HDD](https://gbatemp.net/threads/wii-u-internal-storage-upgrades-are-possible.635629/) using a [SATA adapter](https://github.com/ashquarky/usata/). This option doesn't have size limitation other than the WFS limit of 2TB.


## Thanks

- [GaryOrderNichts](https://github.com/GaryOderNichts) for the IOSU patch to keep IOSU from crashing
- [Ash](https://github.com/ashquarky) for the IOSU patch to enable format
- [V10lator](https://github.com/V10lator) for quota creation and improving title install
- [Lazr1026](https://github.com/Lazr1026) for testing and debugging
