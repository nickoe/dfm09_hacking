# DFM09 hacking
Some noes on the DMF09 radiosonde and a bit of firmware for mucking
around. It uses libopencm3 as a submodule of this project, you can
clone with:

```
git clone --recursive https://github.com/nickoe/dfm09_hacking.git
```

And build firmware with

```
cd firmware && make
```

## Structure

In this repo are various useful notes abour the radiosonde. Includinf
some forum posts from some users on the internet made as a backup and
a link back to the original source. Thank you, the posters, of this
information, it really helped making this way more accessible.

![overview](DSCN6143_pinouts_now_with_JTAG.png)

## References

Some random references to the DFM09 on the internet

* http://radiosonde.eu/RS00-D/RS03J09-D.html
* http://happysat.nl/DFM-09/DFM09.html
* http://zauberkopf.com/20130725_umbau_dfm6/

Potential firmware:

* https://github.com/oe5hpm/dxlAPRS
* https://github.com/df8oe/RS41HUP
