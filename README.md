# cppneofetch

This Repo inspired from [ss7m/paleofetch](https://github.com/ss7m/paleofetch/)

You can config what you want to show on the screen or change the logo (except images).

## Requirement

- libx11
- make
- g++

## Install
If you are using debian based distro you need to uncomment line 42th and comment line 43th befor install.

```
sudo make install
```

## Custom logo

I use a std::string data type vector to store the logo, you can also uncomment any LOGO or create a LOGO for yourself to use (make sure your LOGO has the same pattern as mine).

If I have a logo like this:

```
TTTTTTTTTTTT hhhh                                   hhhh
T::::::::::T h::h                                   h::h
T::::::::::T h::h                                   h::h
T:TT::::TT:T h::h                                   h::h
TT  T::T  TT h::h          aaaaaaaaa   nnn  nnnnnn  h::h
    T::T     h:::hhh       a::::::::a  n::nn::::::n h:::hhh
    T::T     h::::::hh     aaaaaaa:::a n::::::::::n h::::::hh
    T::T     h::::h:::h           a::a nn:::::::::n h::::h:::h
    T::T     h:::h h:::h    aaaaaa:::a  n:::nnn:::n h:::h h:::h
    T::T     h::h   h::h   a:::::::::a  n::n   n::n h::h   h::h
    T::T     h::h   h::h  a::aaaa::::a  n::n   n::n h::h   h::h
    T::T     h::h   h::h a::a    a:::a  n::n   n::n h::h   h::h
  TT::::TT   h::h   h::h a::a    a:::a  n::n   n::n h::h   h::h
  T::::::T   h::h   h::h a:::aaaa::::a  n::n   n::n h::h   h::h
  T::::::T   h::h   h::h  a::::::::a::a n::n   n::n h::h   h::h
  TTTTTTTT   hhhh   hhhh   aaaaaaaa aaa nnnn   nnnn hhhh   hhhh
```
After pasting it to [logo.hpp](https://github.com/Nhat-Thanh/cppneofetch/blob/main/logos.hpp), it pattern like this:
```
std::vector<std::string> LOGO = {
" TTTTTTTTTTTT hhhh                                   hhhh         ",
" T::::::::::T h::h                                   h::h         ",
" T::::::::::T h::h                                   h::h         ",
" T:TT::::TT:T h::h                                   h::h         ",
" TT  T::T  TT h::h          aaaaaaaaa   nnn  nnnnnn  h::h         ",
"     T::T     h:::hhh       a::::::::a  n::nn::::::n h:::hhh      ",
"     T::T     h::::::hh     aaaaaaa:::a n::::::::::n h::::::hh    ",
"     T::T     h::::h:::h           a::a nn:::::::::n h::::h:::h   ",
"     T::T     h:::h h:::h    aaaaaa:::a  n:::nnn:::n h:::h h:::h  ",
"     T::T     h::h   h::h   a:::::::::a  n::n   n::n h::h   h::h  ",
"     T::T     h::h   h::h  a::aaaa::::a  n::n   n::n h::h   h::h  ",
"     T::T     h::h   h::h a::a    a:::a  n::n   n::n h::h   h::h  ",
"   TT::::TT   h::h   h::h a::a    a:::a  n::n   n::n h::h   h::h  ",
"   T::::::T   h::h   h::h a:::aaaa::::a  n::n   n::n h::h   h::h  ",
"   T::::::T   h::h   h::h  a::::::::a::a n::n   n::n h::h   h::h  ",
"   TTTTTTTT   hhhh   hhhh   aaaaaaaa aaa nnnn   nnnn hhhh   hhhh  "
};
```
You must also comment other LOGO in [logo.hpp](https://github.com/Nhat-Thanh/cppneofetch/blob/main/logos.hpp).

## References
- [ss7m/paleofetch](https://github.com/ss7m/paleofetch/)
- [storsui/paleofetch-debian-arch](https://github.com/storsui/paleofetch-debian-arch)


