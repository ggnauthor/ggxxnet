#!/usr/bin/perl --
# !usr/local/bin/perl

# Permission Setting
$g_per_nodelist	= 0600;
$g_per_repdir	= 0701;
$g_per_replay	= 0600;
$g_per_lockdir	= 0701;

# dat file
$g_datfile = "nodelist.dat";
$g_repdir  = "replay";

# node list
$g_nodeList;

# remote info
$g_ownNode;

# rc4
#use Crypt::RC4;
#$g_rc4pass = "aaa";

# connect time
$g_removeTime_idle = 70;	# 70sec  (通常60秒でクライアントから再入される)
$g_removeTime_busy = 600;	# 600sec (busyは再エントリされない、600秒あれば十分)

# lock dir
$g_lockdir = "lock";

##########################################################
if ($ENV{'REQUEST_METHOD'} eq POST)
{
	print "content-type: application/octet-stream\n\n";
	
	read(STDIN, $in, $ENV{'CONTENT_LENGTH'});
	#($name, $value) = split(/=/, $in);
	#$value =~ tr/+/ /;
	#$value =~ s/%([0-9a-fA-F][0-9a-fA-F])/pack("C", hex($1))/eg;
	
	(my$info, my$data) = split(/data=/, $in);
	@args = split(/\|/, $info);
	my$cmd;
	my$name;
	my$size;
	my$port;
	my$param;
	my$win = 0;
	foreach (0..@args-1)
	{
		(my$op, my$value) = split(/=/, $args[$_]);
		if ($op eq "cmd")  { $cmd  = $value; }
		if ($op eq "name") { $name = $value; }
		if ($op eq "size") { $size = $value; }
		if ($op eq "port") { $port = $value; }
		if ($op eq "param"){ $param= $value; }
		if ($op eq "win")  { $win  = $value; }
	}
	
	if ($cmd eq "enter")
	{
		# 入室
		&enter($port, $name, $param, $win);
	}
	elsif ($cmd eq "read")
	{
		# 最新ノードリストの要求
		&read();
	}
	elsif ($cmd eq "leave")
	{
		# 退室
		&leave($port, $name);
	}
	elsif ($cmd eq "repup")
	{
		# Replayアップロード
		&repup($name, $data, $size);
	}
	elsif ($cmd eq "repdown")
	{
		# Replayダウンロード
		&repdown($name);
	}
	elsif ($cmd eq "replist")
	{
		# Replayリスト取得
		&replist();
	}
}
else
{
	my$char = $ENV{'QUERY_STRING'};
	
	my$count = &usercount();
	
	if (1)
	{
		# ユーザカウントText
		print "content-type: text/html\n\n";
		print "user count = ".$count;
	}
	else
	{
		my$num = 0;
		if ($char == 1) { $num = $count % 10; }
		elsif ($char == 1) { $num = $count / 10 % 10; }
		elsif ($char == 2) { $num = $count / 100 % 10; }

		# ユーザカウントImage
		print "Content-type: image/gif\n\n";
		open(IMG,"img/".$num.".gif");
		binmode(IMG);
		binmode(STDOUT);
		print <IMG>;
		close(IMG);
	}
}
exit;

#########################################
sub enter # 入室
{
my$p_port = $_[0];
my$p_name = $_[1];
my$p_param= $_[2];
my$p_win  = $_[3];

	$g_ownNode = &getOwnNode($p_port);
	if ($g_ownNode == 0)
	{
		# print "port error.";
		exit;
	}
	# print "own node = $g_ownNode<br>";
	
	&lockdat();
	&readNodeList();
	
	my$node = $p_name.'@'.$g_ownNode;
	my$i = &findNode($node);
	if ($i != -1)
	{
		# あれば更新
		$g_nodeList[$i] = time().'$'.$node.'%'.$p_param.'#'.$p_win;
	}
	else
	{
		# 無ければ追加
		push(@g_nodeList, time().'$'.$node.'%'.$p_param.'#'.$p_win);
	}
	
	&writeNodeList();
	&unlockdat();
	
	print "##head##".$g_ownNode."##foot##\r\n";
}

#########################################
sub read # ノードリストを取得
{
	&readNodeList();
	
	print "##head##\r\n";
	
	foreach (0..@g_nodeList-1)
	{
		my$i = $_;
		my$dlm = index($g_nodeList[$i], '$');
		if ($dlm != -1)
		{
			my$node = substr($g_nodeList[$i], $dlm+1, 9999);
			print $node."\r\n";
			
			# 同じノードを２つ以上出力しない
			#while (1)
			#{
			#	my$j = &findNode($node);
			#	if ($j == -1) { last; }
			#	$g_nodeList[$j] = "";
			#}
		}
	}
	print "##foot##\r\n";
}

#########################################
sub leave # 退室
{
my$p_port = $_[0];
my$p_name = $_[1];

	$g_ownNode = &getOwnNode($p_port);
	if ($g_ownNode == 0)
	{
		# print "port error.";
		exit;
	}
	# print "own node = $g_ownNode<br>";
	
	# $g_ownNode = encrypt($g_ownNode);
	# $g_ownNode = decrypt($g_ownNode);
	
	&lockdat();
	&readNodeList();
	
	while (1)
	{
		my$i = &findNode($p_name.'@'.$g_ownNode);
		if ($i == -1) { last; }
		splice(@g_nodeList, $i, 1);
	}
	
	&writeNodeList();
	&unlockdat();
}

#########################################
sub usercount # ユーザカウント
{
	# 読み込み
	if (-e $g_datfile)
	{
		open(IN, $g_datfile);
		my@list = <IN>;
		return @list;
	}
	else
	{
		return 0;
	}
}

#########################################
sub repup # Replayアップロード
{
my($p_fname) = $_[0];
my($p_data)  = $_[1];
my($p_size)  = $_[2];

	mkdir($g_repdir, $g_per_repdir);
	
	# rpy作成
	$p_fname = time."_".$p_fname;
	open(OUT, "> $g_repdir/$p_fname");
	chmod($g_per_replay, "$g_repdir/$p_fname"); #0644
	
	syswrite(OUT, $p_data, $p_size);
	
	close(OUT);
	
	print "$p_size";
}

#########################################
sub repdown # Replayダウンロード
{
my($p_fname) = $_[0];
	
	print "##HEAD##";

	# rpy読み込み
	open(IN, "$g_repdir/$p_fname");
	binmode(IN);
	binmode(STDOUT);
	print <IN>;
	close(IN);

	print "##FOOT##";
}

#########################################
sub replist # Replayリスト取得
{
	# rpyディレクトリ上の全ファイル読み込み
	opendir(DIR, "$g_repdir");
	@list = grep(!/^\./, readdir(DIR));
	
	foreach (0..@list-1)
	{
		print "$list[$_]\n";
	}
	
	closedir(DIR);
}

#########################################
#sub encrypt # 暗号化
#{
#my$p_str = $_[0];
#	
#	$p_str = RC4($g_rc4pass, $p_str);
#	$p_str = unpack("H*", $p_str);
#	
#	return $p_str;
#}

#########################################
#sub decrypt # 複号化
#{
#my$p_str = $_[0];
#
#	$p_str = pack("H*", $p_str);
#	$p_str = RC4($g_rc4pass, $p_str);
#	
#	return $p_str;
#}

#########################################
sub getOwnNode # 自ノードを取得
{
my$p_port = $_[0];

	# ポートのチェック
	if ($p_port < 1000 || $p_port > 65535) { return 0; }
	return "$ENV{'REMOTE_ADDR'}:$p_port";
}

#########################################
sub readNodeList # datからノードリストを読み込み
{
	# 読み込み
	if (-e $g_datfile)
	{
		open(IN, $g_datfile);
		
		my@dat = <IN>;
		
		foreach $line (@dat)
		{
			chomp($line);
			
			my$dlm = index($line, '$');
			my$plm = index($line, '%');
			if ($dlm != -1 && $plm != -1)
			{
				# 一定時間で削除される（居続けるために定期的に再エントリが必要）
				# 但し、busyは再入されないため削除対象外、
				my$time = substr($line, 0, $dlm);
				my$busy = substr($line, $plm+1, 1);
				my$removeTime = 0;
				if ($busy == 0) { $removeTime = $g_removeTime_idle; }
				else { $removeTime = $g_removeTime_busy; }
				
				if ($time + $removeTime > time())
				{
					push(@g_nodeList, $line);
				}
				# print $busy.$line."\r\n";
			}
		}
		close(IN);
	}
}

#########################################
sub writeNodeList # datにノードリストを書き込み
{
	# 書き込み
	open(OUT, "> $g_datfile");
	chmod($g_per_nodelist, "$g_datfile");
	
	foreach (0..@g_nodeList-1)
	{
		print OUT "$g_nodeList[$_]\n";
	}
	
	close(OUT);
}

#########################################
sub lockdat
{
	# 排他
	foreach (0..3)
	{
		if (mkdir($g_lockdir, $g_per_lockdir)) { last; }
		sleep(1);
	}
}


#########################################
sub unlockdat
{
	# 排他終了
	rmdir($g_lockdir);
}

#########################################
sub findNode # ノードをリストから探す
{
my$p_nodeStr = $_[0];

	foreach (0..@g_nodeList-1)
	{
		my$dlm1 = index($g_nodeList[$_], '$');
		my$dlm2 = index($g_nodeList[$_], '%');
		if ($dlm1 != -1 && $dlm2 != -1)
		{
			my$node = substr($g_nodeList[$_], $dlm1+1, $dlm2 - ($dlm1+1));
			# print "$node<br>";
			if ($node eq $p_nodeStr)
			{
				return $_;
			}
		}
	}
	return -1;
}

#########################################
sub drawNodeList # ノードリストを表示（デバッグ用）
{
	my$count = @g_nodeList;
	print "node count = $count<br>";
	
	foreach (0..@g_nodeList-1)
	{
		print "$_-$g_nodeList[$_]<br>";
	}
}
