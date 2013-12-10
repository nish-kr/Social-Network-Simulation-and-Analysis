#!usr/bin/perl
use IPC::SysV qw(IPC_PRIVATE IPC_RMID IPC_CREAT S_IRWXU);
use Data::Dumper;
use IPC::Msg;
use Term::ANSIColor;
use Term::ReadKey;
%keywords = {'size', 0,'clique',1,'length',2,'shortest',3,'between',4, 'list',5,'importance','6','important',7};
%Universities;
my $ucount=0;
print "Please enter the name of the file\n";
my $nameofthefile=<STDIN>;
chomp($nameofthefile);
open (data, "universities.txt") or die "Couldn't open the file\n";
while (<data>){
	@Universities;
	my $curruniv=$_;
	chomp($curruniv);
	$curruniv=~ tr/A-Z/a-z/;
	$Universities{$curruniv}=$ucount;
	$ucount++;
}

#waking up the analyser

my $pid=fork();
if ($pid==0){
	exec("./analyser");
	print "something wrong\n";
}

# Global Variables

my @univs=keys %Universities;
my $query;
my @querywords;
my $qsnno=0;
=pod
	returns the number of universities in the query
=cut
sub nofunivs{
	my @univslist;
	foreach $qword(@querywords){
		if (exists($Universities{$qword})){
			push(@univslist,$qword);
		}
	}
	return @univslist;
}

=pod

=cut

sub addrest{
	$cmsg=@_[0];
	while (length($cmsg)<128){
		$cmsg=$cmsg . " ";
	}
	return $cmsg;
}


=pod
	returns the entry numbers or empID's in the query
=cut

sub rtnnos{
	my $checkin=$query . " ";
	my @nlist;
	my $curr=0;
	my $getting=0;
	for ($i=0;$i<length($checkin);$i++){
		if (ord(substr($checkin,$i,1))>47 && ord(substr($checkin,$i,1))<58){
			if (substr($checkin,$i-1,1) eq "s"){
			#	print "Assuming that you are talking about students\n";
				push(@nlist,"s");
			}
			if (substr($checkin,$i-1,1) eq "f"){
			#	print "Assuming that you are talking about faculty\n";
				push(@nlist,"f")
			}
			$getting=1;
			$curr=$curr*10+ord(substr($checkin,$i,1))-48;
		}
		else{
			if ($getting==1){
				push(@nlist,$curr);
				$getting=0;
			}
			$curr=0;
		}
	}
	return @nlist;
}

=pod
	sends the message to the analyser stating the question number and the required inputs
=cut

sub sendmessage{
	my $message=@_[0];
	my $msg=msgget(101,IPC_CREAT | 0666);
	if (msgsnd( $msg , pack("l! a*",1,$message) , 0666 )){
#		print "sent message:",$message,"\n";
	}
	else{
#		print "sending message failed";
	}
}

=pod
	recieves the message from the analyser i.e the answer of the query
=cut

sub rcvmsg{
	my $rmsg=msgget(111,IPC_CREAT | 0666);
	my $rcvmsgbuf;
	my $rcv;

	if ($rcv=msgrcv($rmsg,$rcvmsgbuf,256,1,0666)){

	}  

	checkandremove:{
				for ($i=8;$i<256;$i++){
					if (ord(substr($rcvmsgbuf,$i,1))<=0){
						$rcvmsgbuf=substr($rcvmsgbuf,8,$i-8);
						last checkandremove;
					}
				}
	}
	if ($qsnno==1 || $qsnno ==5 ){

		return $rcvmsgbuf;
	}
	if ($qsnno==6){
		if (&contains("1",$rcvmsgbuf)==1){
			print color("blue"),"Yes....\n",color("reset");
		}
		else{
			if (&contains("0",$rcvmsgbuf)==1){
				print color("blue"),"No....\n",color("reset");
			}
		}
	}
	if ($qsnno==3 || $qsnno==2){
		if (&contains("-1",$rcvmsgbuf)==1){
			return (-1);
		}
		else{

			return $rcvmsgbuf;
		}
	}
	if ($qsnno==4){
		return $rcvmsgbuf;
	}
}

=pod
	checks if a word is in the passed list
	method of calling contains($a,@b)
=cut

sub contains{
	my @checkin=@_;
	$length=@checkin;
	for ($i=1;$i<$length;$i++){
		if (@checkin[0] eq @checkin[$i]){
			return 1;
		}
	}
	return 0;
}

=pod

=cut

sub getquery{
	print color("green"),"Enter the query...\n",color("reset");
	print color("green"),"Or q to quit\n",color("reset");
	$query=<STDIN>;
	chomp($query);
	$query=~ tr/A-Z/a-z/;
	@querywords=split(/ /,$query);
	if ($query eq "q"){
		&sendmessage("       end ");
		exit();
	}
	if (&contains("clique", @querywords)==1){
		$qsnno=1;
	}
	if (&contains("shortest",@querywords)==1 ){
		$qsnno=4;
	}
	if (&contains("length",@querywords)==1 && &contains("shortest",@querywords)==1){
		$qsnno=2;
	}
	if (&contains("list", @querywords)==1 && &contains("shortest",@querywords)==1){
		$qsnno=3;
	}
	if (&contains("importance",@querywords)==1){
		$qsnno=5;
	}
	if (&contains("important",@querywords)==1){
		$qsnno=6;
	}
	return $qsnno;
}

=pod
	sort of the main loop for this program (quits if q is pressed)
	gets the query and sends a message to the analyzer regarding the question
	sends the question number(1..6) and the data of the students to it
=cut
my $first=1;
$nameofthefile=&addrest($nameofthefile);
sendmessage($nameofthefile);
for (;;){
start:{
		my $dontsend=0;
		my $keypr;
		if ($first==1){
			print color("red"),"Calculating........\n";
			print "Please don't press anything ...\n";
			print "Anyway I won't consider anything you type. ;-)\n";
			print "Just so you know anything you type will not be printed on the screen\n",color("reset");
			ReadMode 2;
			ReadMode 3;
			$flwr=&rcvmsg();
			$first=0;
		}
		
		while (defined($keypr=ReadKey(-1))){}
		ReadMode 0;
		my $nfunivs=0;
		my $messagedata="";
		my @univsinq;
		$qsnno=&getquery();
		while(length($query)==0){
			print color("blue"),"Come on....\n",color("reset");
			print color("blue"),"Type something atleast...\n",color("reset");
			&getquery();
		}
		while ($qsnno==0) {
			print color("blue"),"Sorry, I Couldn't understand your query\n",color("reset");
			$qsnno=&getquery();
		}
#		the number of univeristy names in the query is loaded

		@univsinq=&nofunivs();
		$nunivsq=@univsinq;
#	consistency checks
		if ($qsnno==1 || $qsnno==5 || $qsnno==6){
			if ($nunivsq<1){
				print color("blue"),"Sorry, But I think your question is incomplete\n",color("reset");
				print color("blue"), "Did U state the university of the student\n",color("reset");
				last start;
			}
			if($nunivsq>1){
				print color("blue"), "I don't understand your queries with multiple universities\n", color("reset");
				last start;
			}
		}
		if ($qsnno ==2 || $qsnno ==3 ){
			if ($nunivsq<2){
				print color("blue"),"Sorry, But I think your question is incomplete\n",color("reset");
				print color("blue"),"Did U state the university of the student\n",color("reset");
				last start;
			}
			if ($nunivsq>2){
				print color("blue"),"I don't understand your queries with multiple universities\n",color("reset");
				last start;
			}
		}
	#	print "2";
#		the IDs of the students and the faculty  are loaded
		@numbersinq=&rtnnos();
		$nnosinq=@numbersinq;
		if ($nnosinq>3){
			if ((@numbersinq[2] eq "s") || (@numbersinq[2] eq "f")){
				$nnosinq--;
			}
		}
		if ((@numbersinq[0] eq "s") || @numbersinq[0] eq "f"){
			$nnosinq--;
		}
		if ($nunivsq < $nnosinq){
			print color("blue"),"I think you messed the data\n",color("reset");
			last start;
		}
		if ($nunivsq > $nnosinq){
			print color("blue"),"I think you messed the data\n",color("reset");
			last start;
		}

#end of consistency checks

	##################################################################

	#						IPC PART	 		   					 #

	##################################################################

		if ($qsnno == 1 || $qsnno ==5 || $qsnno==6){
			if (@numbersinq[0] eq "s"){
				$messagedata="       $qsnno," . @univsinq[0] . ",s," . @numbersinq[1];
			}
			else{
				$messagedata="       $qsnno," . @univsinq[0] . ",f," . @numbersinq[1];
			}
		}
		if ($qsnno == 2 || $qsnno==3 ){
			if (@numbersinq[0] eq @numbersinq[2]){
			if (@numbersinq[0] eq "s"){
			#	print "here\n";
				$messagedata="       $qsnno," . @univsinq[0] . ",s," . @numbersinq[1] . "," . @univsinq[1] . ",s," .@numbersinq[3];
			}
			else{
				$messagedata="       $qsnno," . @univsinq[0] . ",f," . @numbersinq[1] . "," . @univsinq[1] . ",f," .@numbersinq[3];
			}		
			}
			else{
				print color("blue"),"No path exists\n";
				$dontsend=1;
			}
		}
		if ($qsnno==4){
			$messagedata="       $qsnno";
		}
	#	print $messagedata,"&&&&&&&&&&&&&&&&&";
		if (length($messagedata)!=0){
			$messagedata=&addrest($messagedata);
			if ($dontsend!=1){
				&sendmessage($messagedata);					#sends the message to analyser
			}
		}
		$messagedata="";

	#################################################

	#		sending messages over			#

	##################################################
		#print "qsnno:",$qsnno,"\n";
		if ($qsnno==1 || $qsnno ==5 ){
			$ans=&rcvmsg();
			print color("blue"),$ans,":ans\n",color("reset");
		}
		if ($qsnno==2){
			if ($dontsend!=1){
			$ans=&rcvmsg();
			if ($ans==-1){
				print color("blue"),"No path exists between them  :( \n",color("reset");
			}
			else{
				print color("blue"),$ans,"\n",color("reset");
			}
			}
		}
		if ($qsnno==3){
			if ($dontsend!=1){
			$ans=&rcvmsg();
			if ($ans==(-1)){
				print color("blue"),"No path exists between them  :( \n",color("reset");
			}
			else{
				while (!($ans eq "end      ")){
					print color("blue"),$ans,"\n",color("reset");
					$ans=&rcvmsg();
	#				print "returned\n";
				}
			}
			}
		}
		if ($qsnno==4){
			$ans=&rcvmsg();
		#	print $rcvmsg;
			while (!($ans eq "end      ")){
				print color("blue"),$ans,"\n",color("reset");
				$ans=&rcvmsg();
			}
		}
		if ($qsnno==6){
				$ans=&rcvmsg();
		}
	#################################################

	#			recieving messages over				#

	##################################################
	#	print "Entering nextloop\n";
	}
	$qsnno=0;		#set qsnno to 0 so that it doesn't create any error in the next loop
	$dontsend=0;
	#msgctl()
}
