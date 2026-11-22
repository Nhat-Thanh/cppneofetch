# cppneofetch

A reimplementation of neofetch in c++, a product is aiming to practice C++ skills and try new C++ features.

<div align="center">
    <img src=./example.png width=800>
</div>

## Requirement

- make
- g++

## Build
```
make build
```

## Install

Install to your /usr/bin system 
```
sudo make install
```

if you don't want to install it at system level, pass your user level ```bin``` directory (like ```~/.local``` or similar ones) to the install command like below.
NOTE: Make sure that your user level bin path is in the $PATH as well.
```
make install PREFIX="<your path>"
```

## Run

After built via make command, you can see output by running below comamnd
```
make run
```

if you run ```make install``` before, call the binary on your terminal directly.

```
cppneofetch
```

## Custom logo

I defined the logo as a compile-time array of ***same length*** ```std::string_view``` entries and you can change your custom one in [Logo.hpp](https://github.com/Nhat-Thanh/cppneofetch/blob/main/include/utils/Logo.hpp).
There will only a ```ka_Logo```, you need to keeps yours and comment others.

For instance, if I have a logo like this:

```
                  -`                 
                 .o+`                
                `ooo/                
               `+oooo:               
              `+oooooo:              
              -+oooooo+:             
            `/:-:++oooo+:            
           `/++++/+++++++:           
          `/++++++++++++++:          
         `/+++ooooooooooooo/`        
        ./ooosssso++osssssso+`       
       .oossssso-````/ossssss+`      
      -osssssso.      :ssssssso.     
     :osssssss/        osssso+++.    
    /ossssssss/        +ssssooo/-    
  `/ossssso+/:-        -:/+osssso+-  
 `+sso+:-`                 `.-/+oso: 
`++:.                           `-/+/
.`                                 `/
```
After pasted it to [Logo.hpp](https://github.com/Nhat-Thanh/cppneofetch/blob/main/include/utils/Logo.hpp), it will look like this:
```
inline constexpr std::string_view ka_Logo[] = {
    "                  -`                   ",
    "                 .o+`                  ",
    "                `ooo/                  ",
    "               `+oooo:                 ",
    "              `+oooooo:                ",
    "              -+oooooo+:               ",
    "            `/:-:++oooo+:              ",
    "           `/++++/+++++++:             ",
    "          `/++++++++++++++:            ",
    "         `/+++ooooooooooooo/`          ",
    "        ./ooosssso++osssssso+`         ",
    "       .oossssso-````/ossssss+`        ",
    "      -osssssso.      :ssssssso.       ",
    "     :osssssss/        osssso+++.      ",
    "    /ossssssss/        +ssssooo/-      ",
    "  `/ossssso+/:-        -:/+osssso+-    ",
    " `+sso+:-`                 `.-/+oso:   ",
    "`++:.                           `-/+/  ",
    ".`                                 `/  "
};
```
Remember to comment other logos the file.

## TODO
- [ ] Support multi-socket system.
- [ ] Implement a separated pci bus information extractor.
- [ ] Add left and right trim functions for string_view type.
- [ ] Move the `fb_LookupPciDevice` function from `gen-pci-ids-header.py` script generation to header/source files.
- [ ] Implement a `fstr_GetSubstringBetween` but return string_view type.
- [ ] Remove in-body returning.
- [ ] Review the althernative conmmented `sfb_LookupSystemPciIdsFile` function in case the pci.ids file cannot be downloaded to generate PciIds.hpp header
- [ ] Investigate that there're any multi-package-manager systems and update the `CPackageManInfo` class.
- [ ] Fix potential bug: If the cmd take a paths as arguments, `sfstr_ReadProcessName` would retrieve wrong information.
- [ ] Improve performace of `sfstr_CleanCpuName` function.


