# cppneofetch

You can config what you want to show on the screen or change the logo (except images).

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
make install PREFIX="your path"
```

## Run

After built via make command, you can see your output by running below comamnd
```
make run
```

if you run ```make install``` before, you can call the binary on your terminal directly.

```
cppneofetch
```

## Custom logo

I defined the logo as a vector of ***same length*** ```std::string``` instances and you can change your custom one in [Utils/Logo.hpp](https://github.com/Nhat-Thanh/cppneofetch/blob/main/Utils/Logo.hpp).
There will only a ```vecstr_Logo```, you need to keeps yours and comment others.

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
After pasted it to [Utils/Logo.hpp](https://github.com/Nhat-Thanh/cppneofetch/blob/main/Utils/Logo.hpp), it will look like this:
```
std::vector<std::string> vecstr_Logo = {
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
