use List::Util qw( min max );

$arg = join( ' ', @ARGV);

@times = ();
for($i = 0; $i < 5; $i++)
{
  $res = `$arg`;
  if ($res =~ /(.+): (.+)([ ]*s)\n/)
  {
    $str1 = $1;
    $time = $2;
    $str2 = $3;
  }

#  print "$str1|$time|$str2\n";
  
  $time = $time * 1.0;
  @times = ($time, @times);
}

$min_time = min @times;

print $str1.": ".$min_time.$str2."\n";

