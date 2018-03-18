char max( char i,char j )
{
	if( i > j )	return i;
	else return j;
}

int chartmp( char i,char j )
{
	if( i == j ) return 1;
	else return 0;
}

void main(){
	char i1, i2, i3, temp;
	int judge;
	i1 = 'b'; 
	i2 = 'a'; 
	i3 = 'o';
	temp = max( i1, i2 );
	judge = chartmp( temp, i1 );
	if( judge )	temp = max( i1, i3 );
	else temp = max( i2, i3 );
	write(temp);
}