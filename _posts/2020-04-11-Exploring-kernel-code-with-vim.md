---
layout: post
title: "Exploring Kernel code with vim"
---

Here the problem statement is since the Linux kernel has a huge code database we
need an efficient way to browse the source code in vim.  For example we want to
jump to function definitions, function calls, macro definitions etc. inside the
code used for cross compiling for a particular architecture.
Here I have taken the example of arm64 since that is the architecture I deal
with everyday. This finer details in this article would make sense after
significant exploration of kernel code.

Main differences between Linux kernel and regular C project (from developer's point of view) are next:

 - kernel is very big project (so you should choose which code to index)
 - it has architecture dependent code (and you are only interested in one specific architecture at a time; other architectures shouldn't be
   indexed)
 - it has very specific [coding style][1] you should stick to (and vim should be configured to display code accordingly)
 - it doesn't use C standard library, but instead has it's own similar routines (so your index tool shouldn't index libc headers)

## Solution

## Installing indexing tools ##

To navigate kernel code I would advise `cscope` and `ctags` tools. To install them run next command:
{% highlight bash %}
$ sudo aptitude install cscope exuberant-ctags
{% endhighlight %}

A little explanation:

 - `cscope`: will be used to navigate the code (switch between functions, etc.). It's able to jump to symbol definition, find all symbol
   usages, etc.
 - `ctags`: needed for `Tagbar` plugin (will be discussed further) and for `Omni completion` (auto completion mechanism in vim); can be also
   used for navigation. `ctags` is not as a good choice for C code navigation as cscope, because `ctags` is only able to jump to symbol
definition (not to its callers).

## Creating index database ##

Now you should index your kernel source files. There are 2 approaches here: create index manually or use available script in kernel. If you
are not sure which way is best for you, I recommend to go with kernel script, as it does a lot of neat tricks behind the scenes (like
ignoring non-built sources and moving header files on top of the result list).

But first of all, configure and build the kernel for your architecture/board, as built files can be used later to improve indexing process.

### Indexing with `scripts/tags.sh` ###

Kernel has quite good script (`scripts/tags.sh`) for creating kernel index database. One should use `make cscope` and `make tags` rules to
create index, instead of running that script directly.

Example:

    $ make O=. ARCH=arm SUBARCH=omap2 COMPILED_SOURCE=1 cscope tags

where

 - `O=.` - use absolute paths (useful if you want to load created cscope/ctags index files outside of kernel directory, e.g. for development
   of out-of-tree kernel modules). If you want to use relative paths (i.e. you're gonna do development only in kernel dir), just omit that
parameter
 - `ARCH=...` - select CPU architecture to be indexed. See directories under `arch/` for reference. For example, if `ARCH=arm`, then
   `arch/arm/` directory will be indexed, the rest of `arch/*` directories will be ignored
 - `SUBARCH=...` - select sub-architecture (i.e. board-related files) to be indexed. For example, if `SUBARCH=omap2`, only
   `arch/arm/mach-omap2/` and `arch/arm/plat-omap/` directories will be indexed, the rest of machines and platforms will be ignored.
 - `COMPILED_SOURCE=1` - index only compiled files. You are usually only interested in source files used in your build (hence compiled). If
   you want to index also files that weren't built, just omit this option.
 - `cscope` - rule to make cscope index
 - `tags` - rule to make ctags index

### Indexing manually ###

Kernel script (`tags.sh`) might not work correctly or you may want to have more control over indexing process. In those cases you should
index kernel sources manually.

Insights on manual indexing were taken from [here][2].

First you need to create `cscope.files` file which would list all files you want to index. For example, I'm using next commands to list
files for ARM architecture (`arch/arm`), and particularly for OMAP platform (excluding rest of platforms to keep navigation easy):

<!-- language: sh -->

    find    $dir                                          \
            -path "$dir/arch*"               -prune -o    \
            -path "$dir/tmp*"                -prune -o    \
            -path "$dir/Documentation*"      -prune -o    \
            -path "$dir/scripts*"            -prune -o    \
            -path "$dir/tools*"              -prune -o    \
            -path "$dir/include/config*"     -prune -o    \
            -path "$dir/usr/include*"        -prune -o    \
            -type f                                       \
            -not -name '*.mod.c'                          \
            -name "*.[chsS]" -print > cscope.files
    find    $dir/arch/arm                                 \
            -path "$dir/arch/arm/mach-*"     -prune -o    \
            -path "$dir/arch/arm/plat-*"     -prune -o    \
            -path "$dir/arch/arm/configs"    -prune -o    \
            -path "$dir/arch/arm/kvm"        -prune -o    \
            -path "$dir/arch/arm/xen"        -prune -o    \
            -type f                                       \
            -not -name '*.mod.c'                          \
            -name "*.[chsS]" -print >> cscope.files
    find    $dir/arch/arm/mach-omap2/                     \
            $dir/arch/arm/plat-omap/                      \
            -type f                                       \
            -not -name '*.mod.c'                          \
            -name "*.[chsS]" -print >> cscope.files

For x86 architecture (`arch/x86`) you can use something like this:

<!-- language: sh -->

    find    $dir                                          \
            -path "$dir/arch*"               -prune -o    \
            -path "$dir/tmp*"                -prune -o    \
            -path "$dir/Documentation*"      -prune -o    \
            -path "$dir/scripts*"            -prune -o    \
            -path "$dir/tools*"              -prune -o    \
            -path "$dir/include/config*"     -prune -o    \
            -path "$dir/usr/include*"        -prune -o    \
            -type f                                       \
            -not -name '*.mod.c'                          \
            -name "*.[chsS]" -print > cscope.files
    find    $dir/arch/x86                                 \
            -path "$dir/arch/x86/configs"    -prune -o    \
            -path "$dir/arch/x86/kvm"        -prune -o    \
            -path "$dir/arch/x86/lguest"     -prune -o    \
            -path "$dir/arch/x86/xen"        -prune -o    \
            -type f                                       \
            -not -name '*.mod.c'                          \
            -name "*.[chsS]" -print >> cscope.files

Where `dir` variable can have one of next values:

 - `.`: if you are gonna work only in kernel source code directory; in this case those commands should be run from root directory of kernel
   source code
 - **absolute path to your kernel source code directory**: if you are gonna develop some out-of-tree kernel module; in this case script can
   be run from anywhere

I'm using first option (`dir=.`), because I'm not developing any out-of-tree modules.

Now when `cscope.files` file is ready, we need to run actual indexing:

    $ cscope -b -q -k

Where `-k` parameter tells `cscope` to not index C standard library (as kernel doesn't use it).

Now it's time to create `ctags` index database. To accelerate this stage, we're gonna reuse already created `cscope.files`:

    $ ctags -L cscope.files

Ok, `cscope` and `ctags` index databases are built, and you can remove `cscope.files` file, as we don't need it anymore:

    $ rm -f cscope.files

Next files contain index databases (for `cscope` and `ctags`):

    - cscope.in.out
    - cscope.out
    - cscope.po.out
    - tags

Keep them in root of kernel sources directory.

## vim plugins ##

**NOTE**: Further I show how to use *pathogen* for handling Vim plugins. But now that Vim 8 is released, one can use [native package
loading][4] for the same purpose.

Next we are gonna install some plugins for vim. To have a better grasp on it, I encourage you to use **pathogen** plugin. It allows you to
just `git clone` vim plugins to your `~/.vim/bundle/` and keep them isolated, rather than mixing files from different plugins in `~/.vim`
directory.

Install **pathogen** like it's described [here][5].

Don't forget to do next stuff (as it's described at the same link):

>Add this to your `vimrc`:

>     execute pathogen#infect()

> If you're brand new to Vim and lacking a `vimrc`, `vim ~/.vimrc` and paste in the following super-minimal example:

>     execute pathogen#infect()
>     syntax on
>     filetype plugin indent on


## Installing cscope maps for vim ##

Vim already has cscope support in it (see `:help cscope`). You can jump to symbol or file using commands like `:cs f g kfree`. It's not so
convenient though. To accelerate things you can use shortcuts instead (so you can put your cursor on some function, press some key
combination and jump to function). In order to add shortcuts for cscope you need to obtain `cscope_maps.vim` file.

To install it using **pathogen** you can just clone [this][6] repo to your `~/.vim/bundle`:

    $ git clone https://github.com/joe-skb7/cscope-maps.git ~/.vim/bundle/cscope-maps

Now you should be able to navigate between functions and files in vim using shortcuts. Open some kernel source file, put your keyboard
cursor on some function call, and press <kbd>Ctrl</kbd>+<kbd>\</kbd> followed by <kbd>g</kbd>. It should bring you to the function
implementation. Or it can show you all available function implementations, then you can choose which one to use: [![cscope-struct][7]][7].

For the rest of key mappings see [cscope_maps.vim][8] file.

You can also use commands in vim like:

    :cs f g kmalloc

See `:help cscope` for details.

## ctags note ##

ctags still can be useful for navigation, for example when looking for some `#define` declaration. You can put cursor on this define usage
and press <kbd>g</kbd> followed by <kbd>Ctrl</kbd>+<kbd>]</kbd>. See [this answer][9] for details.

## cscope note ##

Next trick can be used to find structure **declaration** in kernel:

    :cs f t struct device {

Note that above command relies on specific struct declaration style (used in kernel), so we know that struct declaration is always has this
form: `struct some_stuct {`. This trick might not work in projects with another coding style.

## out-of-tree modules development note ##

If you are developing out-of-tree module, you will probably need to load `cscope` and `ctags` databases from your kernel directory. It can
be done by next commands in vim (in command mode).

Load external cscope database:

    :cs add /path/to/your/kernel/cscope.out

Load external ctags database:

    :set tags=/path/to/your/kernel/tags

# vimrc #

Some modifications need to be done to your `~/.vimrc` as well, in order to better support kernel development.

First of all, let's highlight 81th column with vertical line (as kernel coding requires that you should keep your lines length at 80
characters max):

    " 80 characters line
    set colorcolumn=81
    "execute "set colorcolumn=" . join(range(81,335), ',')
    highlight ColorColumn ctermbg=Black ctermfg=DarkRed

Uncomment second line if you want to make 80+ columns highlighted as well.

Trailing spaces are prohibited by kernel coding style, so you may want to highlight them:

    " Highlight trailing spaces
    " http://vim.wikia.com/wiki/Highlight_unwanted_spaces
    highlight ExtraWhitespace ctermbg=red guibg=red
    match ExtraWhitespace /\s\+$/
    autocmd BufWinEnter * match ExtraWhitespace /\s\+$/
    autocmd InsertEnter * match ExtraWhitespace /\s\+\%#\@<!$/
    autocmd InsertLeave * match ExtraWhitespace /\s\+$/
    autocmd BufWinLeave * call clearmatches()


## Kernel coding style ##

To make vim respect kernel coding style, you can pull ready to use plugin: [vim-linux-coding-style][10].

## Useful plugins ##

Next plugins are commonly used, so you can find them useful as well:

 - [NERDTree][11]
 - [Tagbar][12]
 - [file-line][13]
 - [vim-airline][14]

Also these are interesting plugins, but you may need to configure them for kernel:

 - [syntastic][15] 
 - [YCM][16]

## Omni completion ##

Vim 7 (and up) already has auto completion support built in it. It calls `Omni completion`. See [:help new-omni-completion][17] for details.

Omni completion works rather slow on such a big project as kernel. If you still want it, you can enable it adding next lines to your
`~/.vimrc`:

    " Enable OmniCompletion
    " http://vim.wikia.com/wiki/Omni_completion
    filetype plugin on
    set omnifunc=syntaxcomplete#Complete
    
    " Configure menu behavior
    " http://vim.wikia.com/wiki/VimTip1386
    set completeopt=longest,menuone
    inoremap <expr> <CR> pumvisible() ? "\<C-y>" : "\<C-g>u\<CR>"
    inoremap <expr> <C-n> pumvisible() ? '<C-n>' :
      \ '<C-n><C-r>=pumvisible() ? "\<lt>Down>" : ""<CR>'
    inoremap <expr> <M-,> pumvisible() ? '<C-n>' :
      \ '<C-x><C-o><C-n><C-p><C-r>=pumvisible() ? "\<lt>Down>" : ""<CR>'
    
    " Use Ctrl+Space for omni-completion
    " https://stackoverflow.com/questions/510503/ctrlspace-for-omni-and-keyword-completion-in-vim
    inoremap <expr> <C-Space> pumvisible() \|\| &omnifunc == '' ?
      \ "\<lt>C-n>" :
      \ "\<lt>C-x>\<lt>C-o><c-r>=pumvisible() ?" .
      \ "\"\\<lt>c-n>\\<lt>c-p>\\<lt>c-n>\" :" .
      \ "\" \\<lt>bs>\\<lt>C-n>\"\<CR>"
    imap <C-@> <C-Space>
    
    " Popup menu hightLight Group
    highlight Pmenu ctermbg=13 guibg=LightGray
    highlight PmenuSel ctermbg=7 guibg=DarkBlue guifg=White
    highlight PmenuSbar ctermbg=7 guibg=DarkGray
    highlight PmenuThumb guibg=Black
    
    " Enable global scope search
    let OmniCpp_GlobalScopeSearch = 1
    " Show function parameters
    let OmniCpp_ShowPrototypeInAbbr = 1
    " Show access information in pop-up menu
    let OmniCpp_ShowAccess = 1
    " Auto complete after '.'
    let OmniCpp_MayCompleteDot = 1
    " Auto complete after '->'
    let OmniCpp_MayCompleteArrow = 1
    " Auto complete after '::'
    let OmniCpp_MayCompleteScope = 0
    " Don't select first item in pop-up menu
    let OmniCpp_SelectFirstItem = 0

And use <kbd>Ctrl</kbd>+<kbd>Space</kbd> for auto completion.

## Eye candy appearance ##

### 256 colors ###

First of all you want to be sure that your terminal supports 256 colors. For example, it can be achieved using [urxvt-256][18] terminal. For
`gnome-terminal` you can just add next line to your `~/.bashrc`:

<!-- language: sh -->

    export TERM="xterm-256color"

Once it's done put next line to your `~/.vimrc`:

    set t_Co=256

### Color scheme ###

Now download schemes you prefer to `~/.vim/colors` and select them in `~/.vimrc`:

    set background=dark
    colorscheme hybrid

Which color scheme to use is strongly opinion based matter. I may recommend [mrkn256][19], [hybrid][20] and [solarized][21] for starters.

### Font ###

There are a lot of good fonts for programming out there. Many programmers on Linux use [Terminus][22] font, you can try it for starters.

## Known shortcomings ##

Some features are still missing in vim.

1. cscope/ctags can't use definitions from `include/generated/autoconf.h` and ignore code that wasn't built. It is still may be useful to
   have all code indexed to use it as reference when coding.
2. There is no macro expansion (well, there **is** some [function][23] out there (based on `gcc -E`), but I'm not sure if it's gonna work
   for kernel).

The only IDE I know to handle those issues is [Eclipse with CDT][24].

##  Using kernel man pages with VIM ##

You can create man pages for kernel functions using the script named kernel-doc which can be found under the scripts directory:

```
$ ./scripts/kernel-doc -man $(find -name '*.c') | ./scripts/split-man.pl /tmp/man

$ ./scripts/kernel-doc -man $(find -name '*.h') | ./scripts/split-man.pl /tmp/man
```

Add the below line to your `~/.vimrc` and hover over any word and press Ctrl-m to view the man page:

    map <C-m> :!man /tmp/man/"<cfile>".9 <CR>

The above has been tested on kernel version 4.19

## For older kernel versions ##

You can create man pages for kernel functions using the kernel build system:

    $ make mandocs
    $ sudo make installmandocs

The first command builds the kernel man pages, and the second installs them (usually into /usr/local/share/man/man9)
As of kernel version 4.4, "make mandocs" will take a long time to complete (about 45 minutes on my machine).

Once the kernel man pages are installed, you can access them from the command line with:

    $ man <symbol>

If the symbol is found in other places than the kernel (such as in libc or in some other man pages), you can add
the man section number for the kernel, which is '9'.

    $ man 9 <symbol>

For example:

    $ man printk
    $ man 9 abs

To see the kernel man page for a symbol under the cursor in vim, use '''Shift-K'''

### References ###

- [This][25] stack overflow answer.

  [1]: https://www.kernel.org/doc/Documentation/process/coding-style.rst
  [2]: http://cscope.sourceforge.net/large_projects.html
  [3]: https://stackoverflow.com/users/2511795/andy-shevchenko
  [4]: https://shapeshed.com/vim-packages/
  [5]: https://github.com/tpope/vim-pathogen
  [6]: https://github.com/joe-skb7/cscope-maps
  [7]: http://i.stack.imgur.com/mckjb.png
  [8]: https://github.com/joe-skb7/cscope-maps/blob/master/plugin/cscope_maps.vim#L52
  [9]: https://stackoverflow.com/a/1749621/3866447
  [10]: https://github.com/vivien/vim-linux-coding-style/commits/master
  [11]: https://github.com/scrooloose/nerdtree
  [12]: https://github.com/majutsushi/tagbar
  [13]: https://github.com/bogado/file-line
  [14]: https://github.com/bling/vim-airline
  [15]: https://github.com/scrooloose/syntastic
  [16]: https://github.com/Valloric/YouCompleteMe
  [17]: http://vimdoc.sourceforge.net/htmldoc/version7.html#new-omni-completion
  [18]: https://en.wikipedia.org/wiki/Rxvt-unicode
  [19]: https://github.com/mrkn/mrkn256.vim
  [20]: https://github.com/w0ng/vim-hybrid
  [21]: https://github.com/altercation/vim-colors-solarized
  [22]: http://terminus-font.sourceforge.net/shots.html
  [23]: http://vim.wikia.com/wiki/Macro_expansion_C/C%2B%2B
  [24]: https://wiki.eclipse.org/HowTo_use_the_CDT_to_navigate_Linux_kernel_source
  [25]: https://stackoverflow.com/questions/33676829/vim-configuration-for-linux-kernel-development/33682137#33682137
