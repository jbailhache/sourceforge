
sub printdem
{
 my $d = shift;
 my $i = 0;
 print "[";
 print $$d[0];
 for ($i=1; $i<@$d; $i++)
 {
  print ",";
  if ($$d[0] eq _sym || $$d[0] eq _var)
   { print ($$d[$i]); }
  else
   { printdem ($$d[$i]); }
 }
 print "]";
}

sub strdem1
{
 my $d = shift;
 my $i = 0;
 my $s = "";
 $s .= "[";
 $s .= $$d[0];
 for ($i=1; $i<@$d; $i++)
 {
  $s .= ",";
  $s .= strdem ($$d[$i]);
 } 
 $s .= "]";
 return $s;
}

$_var = _var;
$_lambda = _lambda;
$_sym = _sym;
$_ap = _ap;
$_as = _as;
$_axm = _axm;
$_rt = _rt;
$_lr = _lr;
$_ctx = _ctx;

sub node 
{
 my $d = shift;
 return $$d[0];
}

sub val 
{
 my $d = shift;
 return $$d[1];
}

sub var 
{
 my $i = shift;
 return [$var, $i];
}

sub lambda 
{
 my $x = shift;
 return [$_lambda, $x];
}

sub arity 
{
 my $d = shift;
 return @$d - 1;
}

sub sd
{
 my $i = shift;
 my $d = shift;
 return $$d[$i+1];
}

sub mkdem
{
 # print "mkdem:", @_, "\n"; 
 $args = [@_];
 # print "mkdem::", $args, "=",strdem($args),"\n";
 return $args;
}

sub ap 
{
 my $f = shift;
 my $a = shift;
 return [$_ap, $f, $a];
}

sub strdem
{
 my $d = shift;
 my $i = 0;
 my $s = "";
 $s .= "[";
 $s .= node($d);
 for ($i=0; $i<arity($d); $i++)
 {
  $s .= ",";
  if ($i == 0 && (node($d) eq $_sym || node($d) eq $_var ||
      node($d) eq $_lr))
   { $s .= sd($i,$d); }
  else
   { $s .= strdem (sd($i,$d)); }
 } 
 $s .= "]";
 return $s;
}

sub shft
{
 my $m = shift;
 my $n = shift;
 my $x = shift;
 if (node($x) eq $_var)
 {
  if (val($x) >= $m)
   { return var(val($x)+1); }
  else
   { return $x; }
 }
 elsif (node($x) eq $_lambda)
  { return lambda (shft ($m+1, $n, sd(0,$x))); }
 elsif (node(x) == $_sym)
  { return $x; }
 else
  { return mkdem (node($x), shft ($m, $n, sd(0,$x)),
                            shft ($m, $n, sd(1,$x))); }
}

sub subst
{
 my $n = shift;
 my $y = shift;
 my $z = shift;
 if (node($y) eq $_var)
 {
  if (val($y) == $n)
   { return $z; }
  elsif (val($y) > $n)
   { return var(val($y)-1); }
  else
   { return $y; }
 }
 elsif (node($y) eq $_lambda)
 {
  my $n1 = $n + 1;
  my $y1 = sd(0,$y);
  my $z1 = shft(0,1,$z);
  my $x = subst ($n+1, sd(0,$y), shft(0,1,$z));
  return lambda($x);
 }
 else
 {
  return mkdem (node($y), subst($n,sd(0,$y),$z),
		          subst($n,sd(1,$y),$z));
 }
}

sub equal 
{
 my $a = shift;
 my $b = shift;
 return strdem($a) eq strdem($b);
}

$_push = _push;

sub apndb
{
 my $a = shift;
 my $b = shift;
 my @ar = (@$a, $b);
 my $r = \@ar;
 return $r;
}

sub lrctx1 
{
 my $r = shift;
 my $ctx = shift;
 my $prog = sd(0,$ctx);
 my $op = $$prog[0];
 my $cont = $$prog[1];
 my $stk = sd(1,$ctx);
 my $e = sd(2,$ctx);
 if ($op eq qt)
  { return mkdem ($_ctx, $$prog[2], 
     [$_push, $$prog[1], sd(1,$ctx)], sd(2,$ctx)); }
 elsif ($op eq evl)
  {
    my $prog1 = $$prog[1];
    my $stk1 = $$stk[1];
    my @aprogr = (@$stk1, $prog1);
    my $progr = \@aprogr; 
    return mkdem ($_ctx, $progr,
	$$stk[2], $e); 
  }
 elsif ($op eq drop)
  {
    return mkdem ($_ctx, $cont, $$stk[2], $e);
  }
 elsif ($op eq dup)
  {
    return mkdem ($_ctx, $cont, [_push, $$stk[1], $stk], $e);
  }
 elsif ($op eq swap)
  {
    return mkdem ($_ctx, $cont, [_push, $$stk[2][1], [_push, $$stk[1], $$stk[2][2]]], $e);
  }
 elsif ($op eq seq)
  {
    # return mkdem ($_ctx, [$$prog[1], [$$prog[2], $$prog[3]]], $stk, $e);
    my $progr = apndb ($$prog[1], apndb ($$prog[2], $$prog[3]));
    return mkdem ($_ctx, $progr, $stk, $e);      
  }
 elsif ($op eq eqthen)
  {
     if (equal ($$stk[1], $$stk[2][1]))
     {
       my $prog1 = $$prog[1];
       my @aprogr = (@$prog1, $$prog[3]);
       my $progr = \@aprogr;
       return mkdem ($_ctx, $progr, $$stk[2][2], $e);
     }
    else
     {
       my $prog2 = $$prog[2];
       my @aprogr = (@$prog2, $$prog[3]);
       my $progr = \@aprogr;
       return mkdem ($_ctx, $progr, $$stk[2][2], $e);
     }
  }
 elsif ($op eq loop)
  {
   my $prog1 = $$prog[1];
   my @aprogr = (@$prog1, $prog);
   my $progr = \@aprogr;
   return mkdem ($_ctx, $progr, $stk, $e);
  }
 elsif ($op eq sdm)
  {
    return mkdem ($_ctx, $cont, 
     [_push, sd(val($$stk[1]),$$stk[2][1]), $$stk[2][2]], $e); 
  }
 elsif ($op eq mkdm)
  {
    return mkdem ($_ctx, $cont, 
     [_push, 
      mkdem (node($$stk[1]), $$stk[2][1], $$stk[2][2][1]), 
      $$stk[2][2][2]], 
     $e); 
  }
 elsif ($op eq nop)
  {
    return mkdem ($_ctx, $cont, $stk, $e);
  }
 elsif ($op eq gce)
  {
    my $prog1 = $$prog[1];
    my @aprogr = (@$prog1, $$prog[2]);
    my $progr = \@aprogr;
    return mkdem ($_ctx, $progr, 
     [_push, mkdem ($_ctx, $$prog[2], $stk, $e), $stk],
     $e);
  }
 elsif ($op eq setctx)
  {
    return $$stk[1];
  }
 elsif ($op eq LR)
  {
    return mkdem ($_ctx, $cont, 
     [_push, lr(val($$stk[1]),$$stk[2][1]), $$stk[2][2]],
     $e);
  }
 elsif ($op eq oi)
  {
    print strdem($$stk[1]);
    $line = <>;
    $instr = '$x = ' . $line . ";";
    eval ($instr);
    return mkdem ($_ctx, $cont, [_push, $x, $$stk[2]], $e);
  } 
 else
  {
   return $ctx;
  }
}

$_seq = seq;

sub lrctx
{
 my $r = shift;
 my $ctx = shift;
 my $prog = sd(0,$ctx);
 my $instr = sd(0,$prog);
 my $op = node($instr); 
 my $cont = sd(1,$prog);
 my $stk = sd(1,$ctx);
 my $e = sd(2,$ctx);
 if ($op eq qt)
  { return mkdem ($_ctx, $cont,
     [$_push, sd(0,$instr), $stk], $e); }
 elsif ($op eq evl)
  {
     return mkdem ($_ctx, mkdem ($_seq, sd(0,$stk), $cont),
 	sd(1,$stk), $e); 
  }
 elsif ($op eq drop)
  {
    return mkdem ($_ctx, $cont, sd(1,$stk), $e);
  }
 elsif ($op eq dup)
  {
    return mkdem ($_ctx, $cont, mkdem($_push, sd(0,$stk), $stk), $e);
  }
 elsif ($op eq swap)
  {
    return mkdem ($_ctx, $cont, mkdem($_push, sd(0,sd(1,$stk)), mkdem($_push, sd(0,$stk), sd(1,sd(1,$stk)))), $e);
  }
 elsif ($op eq seq)
  {
    # return mkdem ($_ctx, [$$prog[1], [$$prog[2], $$prog[3]]], $stk, $e);
    # my $progr = apndb ($$prog[1], apndb ($$prog[2], $$prog[3]));
    # seq (seq a b) c -> seq a (seq b c)
    my $progr = mkdem ($_seq, sd(0,$instr), mkdem ($_seq, sd(1,$instr), $cont));
    return mkdem ($_ctx, $progr, $stk, $e);
  }
 elsif ($op eq eqthen)
  {
     my $progr;
     if (equal (sd(0,$stk), sd(0,sd(1,$stk))))
     {
       # print "egal\n";
       $progr = mkdem ($_seq, sd(0,$instr), $cont);
       # print "progr=", strdem($progr),"\n";
     }
    else
     {
       # print "different\n";
       $progr = mkdem ($_seq, sd(1,$instr), $cont);  
     }
     $ctxr = mkdem ($_ctx, $progr, sd(1,sd(1,$stk)), $e);
     # print "ctxr=", strdem($ctxr), "\n";
     return $ctxr;
  }
 elsif ($op eq loop)
  {
    # print "loop: prog=", strdem($prog),"\n";
    my $progr = mkdem ($_seq, sd(0,$instr), $prog);
    return mkdem ($_ctx, $progr, $stk, $e);
  }
 elsif ($op eq ex)
  {
    my $progr = sd(1,$cont);
    return mkdem ($_ctx, $progr, $stk, $e); 
  }
 elsif ($op eq sdm)
  {
    return mkdem ($_ctx, $cont, 
     [_push, sd(val($$stk[1]),$$stk[2][1]), $$stk[2][2]], $e); 
  }
 elsif ($op eq mkdm)
  {
    return mkdem ($_ctx, $cont, 
     [_push, 
      mkdem (node($$stk[1]), $$stk[2][1], $$stk[2][2][1]), 
      $$stk[2][2][2]], 
     $e); 
  }
 elsif ($op eq nop)
  {
    return mkdem ($_ctx, $cont, $stk, $e);
  }
 elsif ($op eq gce)
  {
    my $progr = mkdem ($_seq, sd(0,$instr), $cont);
    return mkdem ($_ctx, $progr, 
     [_push, mkdem ($_ctx, $cont, $stk, $e), $stk],
     $e);
  }
 elsif ($op eq setctx)
  {
    return $$stk[1];
  }
 elsif ($op eq LR)
  {
    return mkdem ($_ctx, $cont, 
     [_push, lr(val($$stk[1]),$$stk[2][1]), $$stk[2][2]],
     $e);
  }
 elsif ($op eq oi)
  {
    print strdem($$stk[1]);
    $line = <>;
    $instr = '$x = ' . $line . ";";
    eval ($instr);
    return mkdem ($_ctx, $cont, [_push, $x, $$stk[2]], $e);
  }
 elsif ($op eq rd)
  {
   $line = <>;
   $inst = '$x = ' . $line . ";";
   eval($inst);
   return mkdem ($_ctx, $cont, mkdem ($_push, $x, $stk), $e);
  }
 elsif ($op eq pr)
  {
   print strdem(sd(0,$stk));
   return mkdem ($_ctx, $cont, sd(1,$stk), $e);
  } 
 else
  {
   return $ctx;
  }
}

sub lr
{
 my $r = shift;
 my $d = shift;
 if (arity($d) < 0) { return $d; }
 if (node($d) eq $_sym) { return $d; }
 elsif (node($d) eq $_var) { return $d; }
 elsif (node($d) eq $_ap)
  {
   return ap (lr($r,sd(0,$d)), lr($r,sd(1,$d)));  
  }
 elsif (node($d) eq $_axm)
  { return sd($r,$d); }
 elsif (node($d) eq $_rt)
  {
   if (equal (lr(1,sd(0,$d)), lr(1,sd(1,$d))))
    { return lr(0,sd($r,$d)); }
   else
    { return $d; }
  }
 elsif (node($d) eq $_lambda)
  { return lambda(lr($r,sd(0,$d))); }
 elsif (node($d) eq $_as)
  {
   my $d0 = sd(0,$d);
   my $f = lr(1,$d0);
   if ($r == 0 || ! (node($f) eq $_lambda))
    { return ap (lr($r,sd(0,$d)), lr($r,sd(1,$d))); }                   
   else
    { return subst (0, sd(0,$f), lr(1,sd(1,$d))); }
  }
 elsif (node($d) eq $_lr)
  {
   if ($r == 0)
    { return mkdem ($_lr, sd(0,$d), lr($r,sd(1,$d))); }
   else
    { return lr (sd(0,$d), lr($r,sd(1,$d))); }
  }
 elsif (node($d) eq _quote)
  {
   return sd (0, $d);
  }
 elsif (node($d) eq _eval)
  {
   return lr ($r, lr ($r, sd(0,$d)));
  }
 elsif (node($d) eq $_ctx)
  {
   if ($r == 0)
    { return $d; }
   else
    { return lrctx ($r, $d); } 
  }
 else
  { return $d; }
}

sub aps
{
 my $x = shift;
 my $n;
 my $i;
 my @r;
 if (node($x) eq $_ap)
  { $n = $_as; }
 else
  { $n = node($x); }
 @r = ($n);
 for ($i=0; $i<arity($x); $i++)
  {
    if ($i == 0 && (node($x) eq $_sym || node($x) eq $_var ||
      node($x) eq $_lr))
     { push @r, sd($i,$x); }
    else
     { push @r, aps(sd($i,$x)); }
  }
 return [@r];
}

sub red
{
 my $x = shift;
 my $y;
 while (! equal ($x, ($y = aps (lr (1, $x)))))
  { $x = $y; 
    print " -> ", strdem($y), "\n";
  }
 return $y;
}

$d1 = [_ap, [_ap, [a], [b]], [c]];

printdem ($d1); print "\n";
print (strdem1($d1)); print "\n";
print (strdem ($d1)); print "\n";

$f = lambda (var (0));

$toplevel1 = [_ctx, [loop, [seq,[oi],[evl]], [k]], [_push, [Hello], [stk]], [e]];

$toplevel = [_ctx, [seq, [loop, [seq,[rd],[evl]]], [k]], [stk], [e]];

while ($line = <>)
{
 # print "line:", $line, "\n";
 $cmd = '$x = ' . $line . ";";
 # print "cmd=",$cmd,"\n";
 eval($cmd);
 # print "x=", strdem($x), "\n";
 # print "node:", node($x),"\n";
 $l = lr(0,$x);
 $r = lr(1,$x);
 print " ", strdem($l), " = ", strdem($r), "\n";
 print " ", strdem(red($x)),"\n";
}
 

