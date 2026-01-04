When making icons for Windows, it is important to generate the multiple ICO files on different sizes and pack them into one single ICO file.

Using a tool such as `icomake` is recommended for this: https://github.com/tringi/icomake

The recommended workflow is to progressively downscale the icon into the following sizes and export them from a tool that supports exporting to ICO:

- 256.ico, 256x256 
- 128.ico, 128x128
- 64.ico, 64x64
- 32.ico, 32x32
- 16.ico, 16x16

Once these are ready, `icomake` can pack these into one icon with the following command:
```
.\icomake.exe app.ico 16.ico 32.ico 64.ico 128.ico 256.ico
```

If done correctly, Windows will now pick the icon with the correct size on the title bar, the taskbar and as a shortcut.