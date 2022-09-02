int var1;
int var2;
extern int var3;

extern "C" auto entry()
{
	var1 = 420;
	var2 = 69;
	var3 = 10;
	return var1;
}