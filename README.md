# omaVideo

the most unconventional way to play back videos

**NOTE:** this wasn't designed to be used by users if you can't tell from the jankiness of this thing. _if you have no idea how to code, you prob won't know how to use this and i won't help you._

## how to use

1. use FFmpeg to split a video into `frames/%04d.png`
2. edit the `filesToBin.py` file to fit your video's properties (frame count, resolution, FPS)
3. run `filesToBin.py`
4. compile
   - for the Linux frontend, use `cmake .. -DPLATFORM=linux`. this requires Xlib
   - for the EFI frontend, use `cmake .. -DPLATFORM=efi`. make sure you have ran `git submodule update --init` at least once
5. run
   - the Linux frontend will produce a `omaVideo` executable which looks for `video.bin` in the PWD
   - the EFI frontend will produce a `omaVideo.efi` executable which looks for `VIDEO.BIN` in the filesystem root, and a `fat.img` containing `test.bin` from the root as `VIDEO.BIN` along with `omaVideo.efi` as `BOOTX64.EFI`

## known issues

### EFI frontend

- runs slower than optimal. prob needs tweaks in the msleep or get_ms_time functions
- doesn't have proper formatting support in the log function because lol no libc also fuck wchar
