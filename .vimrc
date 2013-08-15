" vim:shiftwidth=2:tabstop=2:softtabstop=2:et

set nocompatible
colorscheme ron
syntax on
set enc=utf-8
set tenc=utf-8
set fileencodings=utf-8,gbk
set nobackup

set shiftwidth=4
set tabstop=4
"set expandtab
"set softtabstop=4

set backspace=indent,eol,start
set history=50
set showcmd
set number
set wrap
set incsearch
set ignorecase
set smartcase
set hlsearch
set wildmenu
filetype plugin indent on

" indent
set autoindent
set cindent
set cino=:0,l1,g0,p0,t0

set completeopt=longest,menu

" show encoding in status bar
set laststatus=2
set statusline=%<%f\ %h%m%r%=%k[%{(&fenc==\"\")?&enc:&fenc}%{(&bomb?\",BOM\":\"\")}]\ %-14.(%l,%c%V%)\ %P
if has('multi_byte') && v:version > 601
  if v:lang =~? '^\(zh\)\|\(ja\)\|\(ko\)'
    set ambiwidth=double
  endif
endif

" taglist
let Tlist_Show_One_File=1
let Tlist_Exit_OnlyWindow=1
nmap <silent> <F2>            :TlistToggle<CR>
imap <silent> <F2>            <C-O>:TlistToggle<CR>

" NERDTree
let NERDTreeIgnore = ['tags', '\.swp$', '\.o', '\.log', '\~$']
nmap <silent> <F3>            :NERDTreeToggle<CR>
imap <silent> <F3>            <C-O>:NERDTreeToggle<CR>

" quickfix
augroup QuickfixToggle
 autocmd!
 autocmd BufWinEnter quickfix let g:qfix_win = bufnr("$")
 autocmd BufWinLeave * if exists("g:qfix_win") && expand("<abuf>") == g:qfix_win | unlet! g:qfix_win | endif
augroup END
function! QuickfixToggle()
  if exists("g:qfix_win")
    cclose
  else
    copen
  endif
endfunction
nmap <F1> :echo<CR>
imap <F1> <C-o>:echo<CR>
nmap <silent> <F4>            :call QuickfixToggle() <CR>
imap <silent> <F4>            <C-O>:call QuickfixToggle() <CR>
nmap <F11>                    :cp<CR>
imap <F11>                    <C-O>:cp<CR>
nmap <F12>                    :cn<CR>
imap <F12>                    <C-O>:cn<CR>

nmap <F5>                     :update<CR>:A<CR>
imap <F5>                     <Esc>:update<CR>:A<CR>

set pastetoggle=<F6>

" Doxygen
let g:DoxygenToolkit_authorName="Xiang Wen"
let g:DoxygenToolkit_authorMail="wxever@pku.edu.cn"
let g:DoxygenToolkit_briefTag_funcName="yes"
let g:DoxygenToolkit_versionString="1.0"
let g:DoxygenToolkit_licenseTag="Copyright (C) 2011 - Peking University"
let g:DoxygenToolkit_commentType = "C++"
map  <F9><F9>                 :set syntax=cpp.doxygen<CR>
imap <F9><F9>                 <C-O>:set syntax=cpp.doxygen<CR>
map  <F9>t                    :DoxLicAuthor<CR>
imap <F9>t                    :DoxLicAuthor<CR>
map  <F9>a                    :DoxAuthor<CR>
imap <F9>a                    <C-O>:DoxAuthor<CR>
map  <F9>l                    :DoxLic<CR>
imap <F9>l                    <C-O>:DoxLic<CR>
map  <F9>f                    :Dox<CR>
imap <F9>f                    <C-O>:Dox<CR>
map  <F9>b                    :DoxBlock<CR>
imap <F9>b                    <C-O>:DoxBlock<CR>
map  <F9>c                    O/** */<Left><Left>
imap <F9>c                    <Esc>O/** */<Left><Left>

map  <F10>                    :set wrap<CR>
imap <F10>                    <C-O>:set wrap<CR>
map  <F10><F10>               :set nowrap<CR>
imap <F10><F10>               <C-O>:set nowrap<CR>


let mapleader=','
let g:C_MapLeader=','

" cscope
nmap <leader>ss               :cs find s <C-R>=expand("<cword>")<CR><CR>
nmap <leader>sg               :cs find g <C-R>=expand("<cword>")<CR><CR>
nmap <leader>sc               :cs find c <C-R>=expand("<cword>")<CR><CR>
nmap <leader>st               :cs find t <C-R>=expand("<cword>")<CR><CR>
nmap <leader>se               :cs find e <C-R>=expand("<cword>")<CR><CR>
nmap <leader>sf               :cs find f <C-R>=expand("<cfile>")<CR><CR>
nmap <leader>si               :cs find i <C-R>=expand("%:t")<CR><CR>
nmap <leader>sd               :cs find d <C-R>=expand("<cword>")<CR><CR>

" hlsearch
nmap <leader>hl               :nohlsearch<CR>
imap <leader>hl               <C-O>:nohlsearch<CR>

" save & quit
nmap <leader>a                :confirm qa<CR>
nmap <leader>d                :confirm bd<CR>
nmap <leader>q                :confirm q<CR>
nmap <leader>w                :w<CR>
imap <leader>w                <C-O>:w<CR>

" make
nmap <leader>m                :update<CR>:make<CR>
imap <leader>m                <Esc>:update<CR>:make<CR>

" Key mappings to ease browsing long lines
noremap  <Down>                    gj
noremap  <Up>                      gk
inoremap <Down>               <C-O>gj
inoremap <Up>                 <C-O>gk

" code complete
imap <leader>f                <C-X><C-F>
inoremap <C-]>                <C-X><C-]>

nmap <leader>tt               <ESC>:silent !ct ..<CR> :redraw!<CR>
imap <leader>tt               :silent !ct ..<CR> :redraw!<CR>

" Smart way to move btw. windows
nmap <C-j>                    <C-W>j
nmap <C-k>                    <C-W>k
nmap <C-h>                    <C-W>h
nmap <C-l>                    <C-W>l

" buffer
nmap <leader>,                :bn<CR>
nmap <leader>.                :bp<CR>

" indent all
nmap <leader>bb               ma gg=G `a
imap <leader>bb               <ESC>ma gg=G `a

function! GnuIndent()
  setlocal cinoptions=>4,n-2,{2,^-2,:2,=2,g0,h2,p5,t0,+2,(0,u0,w1,m1
  setlocal shiftwidth=2
  setlocal tabstop=8
endfunction

function! RemoveTrailingSpace()
  if $VIM_HATE_SPACE_ERRORS != '0' &&
        \(&filetype == 'c' || &filetype == 'cpp' || &filetype == 'vim' || &filetype == 'docbk')
    normal m`
    silent! :%s/\s\+$//e
    normal ``
  endif
endfunction

" Highlight space errors in C/C++ source files (Vim tip #935)
if $VIM_HATE_SPACE_ERRORS != '0'
  let c_space_errors=1
endif

" visul mode * # search
function! VisualSearch(direction) range
  let l:saved_reg = @"
  execute "normal! vgvy"
  let l:pattern = escape(@", '\\/.*$^~[]')
  let l:pattern = substitute(l:pattern, "\n$", "", "")
  if a:direction == 'b'
    execute "normal ?" . l:pattern . "^M"
  else
    execute "normal /" . l:pattern . "^M"
  endif
  let @/ = l:pattern
  let @" = l:saved_reg
endfunction

"Basically you press * or # to search for the current selection !! Really useful
vnoremap <silent> * :call VisualSearch('f')<CR>
vnoremap <silent> # :call VisualSearch('b')<CR>

" http://vim.wikia.com/wiki/VimTip80
" Tell vim to remember certain things when we exit
"  '10 : marks will be remembered for up to 10 previously edited files
"  "100 : will save up to 100 lines for each register
"  :20 : up to 20 lines of command-line history will be remembered
"  % : saves and restores the buffer list
"  n... : where to save the viminfo files
set viminfo='10,\"100,:20,%,n~/.viminfo

if has('autocmd')
  au FileType cxx setf cpp
  au FileType hpp setf cpp
  au BufEnter /usr/include/c++/*  setf cpp

  au FileType cpp setlocal syntax=cpp.doxygen
  au FileType c setlocal syntax=c.doxygen

  au FileType python set et |set shiftwidth=4| set softtabstop=4

  " Recognize standard C++ headers
  au FileType cpp setlocal path+=/usr/include/c++/4.1.2
  au FileType cpp setlocal tags+=$HOME/.vim/tags/cpp_tags

  " Setting for files following the GNU coding standard
  au BufEnter /usr/*                call GnuIndent()

  " Remove trailing spaces for C/C++ and Vim files
  au BufWritePre *                  call RemoveTrailingSpace()

  " for code complete plugin
  imap <leader>... ".g:rs."...".g:re."
  " docbook
  imap <leader>docbk <?xml version="1.0"?><CR><!DOCTYPE book PUBLIC "-//OASIS//DTD DocBook XML V4.2//EN">
  au BufEnter *.sgm    setf docbk
  au BufEnter *.xml    setf docbk


  " when we reload, tell vim to restore the cursor to the saved position
  augroup JumpCursorOnEdit
    au!
    autocmd BufReadPost *
          \ if expand("<afile>:p:h") !=? $TEMP |
          \ if line("'\"") > 1 && line("'\"") <= line("$") |
          \ let JumpCursorOnEdit_foo = line("'\"") |
          \ let b:doopenfold = 1 |
          \ if (foldlevel(JumpCursorOnEdit_foo) > foldlevel(JumpCursorOnEdit_foo - 1)) |
          \ let JumpCursorOnEdit_foo = JumpCursorOnEdit_foo - 1 |
          \ let b:doopenfold = 2 |
          \ endif |
          \ exe JumpCursorOnEdit_foo |
          \ endif |
          \ endif
    " Need to postpone using "zv" until after reading the modelines.
    autocmd BufWinEnter *
          \ if exists("b:doopenfold") |
          \ exe "normal zv" |
          \ if(b:doopenfold > 1) |
          \ exe "+".1 |
          \ endif |
          \ unlet b:doopenfold |
          \ endif
  augroup END


endif

"OmniComplet
let OmniCpp_DefaultNamespaces = ["std"]

"lcc:
