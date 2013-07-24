read LINE
dir=$LINE
read LINE
cmd=$LINE
cd $dir
$cmd &
echo $!
