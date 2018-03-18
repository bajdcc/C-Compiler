void main()
{
	int i;
	i = 0;
	
label:
	i = i + 1;
	if( i < 10 ) { goto label; }
}