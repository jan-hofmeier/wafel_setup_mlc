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


## Building

```bash
export STROOPWAFEL_ROOT=/path/too/stroopwafel-repo
make
```

## Thanks

- GaryOderNichts for the IOSU patch to keep IOSU from crashing
- quarky for the IOSU patch to enable format
- V10lator for quota creation and improving title install
- Lazr1026 for testing
