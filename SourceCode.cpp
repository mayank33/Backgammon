#include<stdio.h>
#include<iostream>
#include<string>
using namespace std;

#define infinity 10000000
int board[35];
int tmp[35];
int was_opponent,st;
string block_name[25];char bar[20];
int moves11[400][5],moves12[400][5],moves_alice1[400][5],moves_alice2[400][5],moves[5];
int g_pos,nbar,pbar,g_val,bearing_off,init_home_doors;

void copy_array(int src[],int des[]);
void play_bar(int die1,int die2,int des[],int factor,int prev_move[]);
int single(int die,int factor);
void expecti_max(int die1,int die2,int des[]);
double look_deep(int moves[][5],int sz,int max_move);
int check_config(int pos,int die);
void set_bar_var();
void swap_bar_var();
int give_states(int die1,int die2,int moves[][5],int factor,int prev_move[]);
int set_board(int moves[]);
double simply_evaluate(int moves[][5],int sz,int max_move,int factor,int prev_move[]);
void reset_board();
double evaluate(int Board[]);
double evalBearOff(int Board[]);
void reverse_array();
void put_board();
int checkAttack(int BoardState [] ,int pos);
int home_checkers(int BoardState[]);

int check_bearing()
{
	int x;
	for(x=1;x<19 && tmp[x]<=0 ;x++);
	return int(x == 19 && pbar == 0);
}

int check_greedy( int BoardState[] )
{
	int flag = 0,i,j;
	for( i=1; i<=24; i++)
		if( BoardState[i] >= 1 )
			break;
	for( j=i+1; j<=24; j++ )
	{
		if( BoardState[j] < 0 )
		{
			flag = 1;
			break;
		}
	}
	if( flag == 1 )// all opponent checkers are not behind so no greedy
		return 0;
	else 
	{
		if(nbar == 0)
			return 1;// greedy
		else
			return 0;
	}
}

int home_checkers(int BoardState[])
{
	int ct = 0,x;
	for(x=19;x<=24;x++)
	{
		if(BoardState[x] >=2)
			ct += BoardState[x];
	}
	return ct;
}

int move_greedy( int BoardState[] , int r )
{
	for( int i=13; i<=18; i++ )
	{
		if( BoardState[i] > 0 && r != 0 )
		{
			if( i+r >= 19 /*&& i+r <= 20*/ )
			{
				BoardState[i]--;
				BoardState[i+r]++;
				r = 0;
				return i;
			}
		}
	}
	for( int i=7; i<=12; i++)
	{
		if( BoardState[i] !=0 && r!= 0)
		{
			if( i+r >=13 )
			{
				BoardState[i]--;
				BoardState[i+r]++;
				r = 0;
				return i;
			}
		}
	}
	for( int i=13; i<=18; i++)
	{
		if( BoardState[i] !=0 && r!= 0)
		{
			BoardState[i]--;
			BoardState[i+r]++;
			r = 0;  
			return i; 
		}
	}
	for( int i=7; i<=12; i++)
	{
		if( BoardState[i] !=0 && r!= 0)
		{
			BoardState[i]--;
			BoardState[i+r]++;
			r = 0;
			return i;
		}
	}
}

double evalBearOff( int BoardState[] )
{
	double val=0,sum ;
	int flag;

	for( int i=19; i<=24; i++)
		if( BoardState[i] >= 0 )
			val += ( BoardState[i]*(i-18));

	sum = 0;
	for( int i=19;i<=24;i++)
		if( BoardState[i] >= 0 )
			sum += BoardState[i];
	val += 10*(15-sum );

	val += 10*(nbar);

	for( int i=19; i<=24; i++ )
	{
		if( BoardState[i] == 1 )
		{
			val -= 5;
			if( nbar != 0 )
				val -= 100;
			else
			{
				for(int j=i+1; j<= 24; j++)
				{
					if( BoardState[j] < 0 )
						flag = 1;
				}
				if( flag == 1 )
					val =- 100;
			}	
		}
	}			
	return val;
}

double evaluate( int BoardState[] )
{

	if(bearing_off == 1)
		return evalBearOff(BoardState);

	double var = 0.0;
	int p1 = 0, p2 = 0,count;
	var += 10.5 * nbar;
	var += 5.5*(nbar - pbar);

	for( int i=1; i<=24; i++ )
	{
		if( BoardState[i] > 1 )
			p1++;
		else if( BoardState[i] < -1 )
			p2++;
	}
	var += 5.0*(p1 - p2);
	var += 2.5*(p1 - p2) + 2.525*( nbar - pbar);
	int m=0 , n=0;
	for( int i=1; i<=24; i++)
	{
		if( BoardState[i] > 1 )
		{
			n += 2;
			if( BoardState[i] > 4 )
				n -= 1;
			if( i >=19 && i <= 24 )
				n += 1;
		}
		if( BoardState[i] < -1 )
		{
			m -= 2;
			if(BoardState[i] < -4)
				m += 1;
		}
	}
	var += 1.0*(m+n) + 2.90 * (nbar - pbar);

	int final_home_doors = home_checkers(BoardState);
	if(final_home_doors == init_home_doors)
		var -= 20;

	for( int i=1; i<=24; i++)
	{
		if( BoardState[i] == 1 )
		{
			int res = checkAttack( BoardState, i );
			if( res == 1 )
				var -= (i*3);
		}
	}

	int x;
	for( x=1; x<=6; x++) 
	{ 
		count = 0;
			for( int j=x+1; j<x+6 && j<=24; j++ ) 
			{ 
				if( BoardState[j] < -1 ) 
					count++; 
			} 
			if( count >= 3) 
			{
				if(BoardState[x] == 1)
					var -= 80;  
				else
					var -= 60;
			}
	}

	return var;
}

int checkAttack( int BoardState[], int pos )
{
	for( int i=pos+1; i<=pos+8; i++ )
	{
		if( i<= 24 && BoardState[i] < 0 )
			return 1;
	}
	if( pos >=19 && pos <= 24 && nbar != 0)
		return 1;
	return 0;
}

int main()
{
	int moves[5],x,die1,die2,array[5];
	char ch;
	for(x=1;x<=24;x++)
		scanf("%d",&board[x]);
//	cin >> bar;
	scanf("%c",&ch);
	scanf("%c",&ch);
	x = 0;
	while(ch!='\n')
	{
		bar[x++] = ch;
		scanf("%c",&ch);
	}
	bar[x]='\0';
	//cout << "--"<<bar<<".."<<endl;
	scanf("%d%d",&die1,&die2);

	set_bar_var();
	reset_board();
	bearing_off = 0;
//	put_board();

	init_home_doors = home_checkers(board);

	if(pbar >= 1)
		play_bar(die1,die2,moves,1,array);
	else
		expecti_max(die1,die2,moves);

	if(moves[1] == 0)
		printf("Z %d\n",moves[0]);
	else if(moves[1] == -1 || moves[0] == -1)
		printf("pass\n");
	else
		printf("%d %d\n",moves[1],moves[0]);
	if(moves[3] == 0)
		printf("Z %d\n",moves[2]);
	else if(moves[3] == -1 || moves[2] == -1)
		printf("pass\n");
	else
		printf("%d %d\n",moves[3],moves[2]);
}

void reset_board()
{
	int x;
	for(x=1;x<=24;x++)
		tmp[x] = board[x];
}

void play_bar(int die1,int die2,int des[],int factor,int prev_move[])
{
	int bar_move1[5],bar_move2[5],a;
	int val1,val2,fl1,fl2,max_move1,max_move2;
	fl2 = 0;val1 = -infinity;val2 = -infinity;

	fl1 = single(die1,factor);
	bar_move1[1] = g_pos;
	if(g_pos == 0)
		bar_move1[0] = die1;
	else if(g_pos > 0)
		bar_move1[0] = g_pos + die1;
	else if(g_pos == -1)
		bar_move1[0] = -1;

	fl2 = single(die2,factor);
	bar_move1[3] = g_pos;
	if(g_pos == 0)
		bar_move1[2] = die2;
	else if(g_pos > 0)
		bar_move1[2] = g_pos + die2;
	else if(g_pos == -1)
		bar_move1[2] = -1;
	val1 = g_val;
	max_move1 = fl1 + fl2;

	reset_board();
	set_bar_var();
	if(factor == -1)
	{
		set_board(prev_move);
		for(a=1;a<=24;a++)
			tmp[a] = -tmp[a];
		reverse_array();
		swap_bar_var();
	}
	fl1 = single(die2,factor);
	bar_move2[1] = g_pos;
	if(g_pos == 0)
		bar_move2[0] = die2;
	else if(g_pos > 0)
		bar_move2[0] = g_pos + die2;
	else if(g_pos == -1)
		bar_move2[0] = -1;

	fl2 = single(die1,factor);
	bar_move2[3] = g_pos;
	if(g_pos == 0)
		bar_move2[2] = die1;
	else if(g_pos > 0)
		bar_move2[2] = g_pos + die1;
	else if(g_pos == -1)
		bar_move2[2] = -1;
	val2 = g_val;
	max_move2 = fl1 + fl2;

	if(max_move1 == max_move2)
	{
		if(val1 > val2)
			copy_array(bar_move1,des);
		else
			copy_array(bar_move2,des);
	}
	else if(max_move1 > max_move2)
		copy_array(bar_move1,des);
	else
		copy_array(bar_move2,des);
}

int single(int die,int factor)
{
	int st_pos,st_x,end_pos,end_x;
	int x,side,pos,ret;
	int bar_move[2];
	double val;
	bar_move[0] = -1;
	g_pos = -1;
	g_val = -infinity*1.0;
	if(pbar > 0)
	{
		if(tmp[die] >= -1)
		{
			g_pos = 0;
			tmp[die] ++;
			pbar --;
			if(tmp[die] == 0)
			{
				nbar ++;
				tmp[die] ++;
			}
			g_val = evaluate(tmp);
			return 1;
		}
		return 0;
	}
	for(x=1;x<=24;x++)
	{
		pos = x + die;
/*		if(x == 22)
		{
			cout << tmp[x] <<endl;
			put_board();
		}*/
		if(tmp[x] < 1 || pos>24 || tmp[pos] < -1)
			continue;
		tmp[x] --;
		tmp[pos] ++;
		was_opponent = 0;
		if(tmp[pos] == 0)
		{
			was_opponent = 1;
			nbar ++;
			tmp[pos] ++;
		}
		val = evaluate(tmp);
		tmp[x] ++;
		tmp[pos] --;
		if(was_opponent == 1)
		{
			nbar --;
			tmp[pos] --;
		}
		if(val > g_val)
		{
			g_pos = x;
			g_val = val;
		}
	}
	if(g_pos == -1)
		return 0;
	return 1;
}

void set_bar_var()
{
	int x,ct;
	ct = 0;
	nbar = 0;pbar = 0;
//	printf("here\n");
	for(x=0;bar[x]!='\0';x++)
	{
	/*	if(ct == 1)
			nbar = nbar*10 + bar[x] - '0';
		if(ct == 0)
			pbar = pbar*10 + bar[x] - '0';
		if(bar[x] == ' ')
			ct ++;*/
		if(bar[x] == 'a')
			pbar ++;
		if(bar[x] == 'b')
			nbar ++;
	}
//	printf("pbar %d nbar %d\n",pbar,nbar);
}

void swap_bar_var()
{
	nbar = pbar + nbar;
	pbar = nbar - pbar;
	nbar = pbar - nbar;
}

void reverse_array()
{
	int a,swp;
	for(a=1;a<=12;a++)
	{
		swp = tmp[a];
		tmp[a] = tmp[24-a+1];
		tmp[24-a-1] = swp;
	}
}

double simply_evaluate(int moves[][5],int sz,int max_move,int factor,int prev_move[])
{
	int x,a;
	double val,ret_val;
	ret_val = -infinity*1.0;
	for(x=0;x<sz;x++)
	{
		if(moves[x][4] < max_move)
			continue;
		reset_board();
		if(factor == -1)
		{
			set_bar_var();
			set_board(prev_move);
			for(a=1;a<=24;a++)
				tmp[a] = -tmp[a];
			reverse_array();
			swap_bar_var();
		}

		if(moves[x][0] != -1)
		{
			tmp[moves[x][0]] ++;
			if(tmp[moves[x][0]] == 0)
				tmp[moves[x][0]] ++;
			tmp[moves[x][1]] --;
		}
		if(moves[x][2] != -1)
		{
			tmp[moves[x][2]] ++;
			if(tmp[moves[x][2]] == 0)
				tmp[moves[x][2]] ++;
			tmp[moves[x][3]] --;
		}
		val = evaluate(tmp);
		if(val > ret_val)
		{
			ret_val = val;
			g_pos = x;
		}
	}
	return ret_val;
}

void copy_array(int src[],int des[])
{
	int x;
	for(x=0;x<4;x++)
		des[x] = src[x];
}

void expecti_max(int die1,int die2,int des[])
{
	int pos1,pos2,max_move1,max_move2,len1,len2,move1,move2;
	double val1,val2;
	int array[2];
	array[0] = 11;
	reset_board();
	bearing_off = check_bearing();

	if(check_greedy(tmp) == 1 && bearing_off == 0)
	{
		//		cout << "in greedy"<<endl;	
		move1 = move_greedy(tmp,die1);
		move2 = move_greedy(tmp,die2);
		des[0] = move1 + die1;
		des[1] = move1;
		des[2] = move2 + die2;
		des[3] = move2;
		return ;
	}

	max_move1 = give_states(die1,die2,moves11,1,array);
	len1 = st;
	reset_board();
	max_move2 = give_states(die2,die1,moves12,1,array);
	len2 = st;

	reset_board();

	if(max_move1 == max_move2)
	{
		if(bearing_off == 1)
		{
			val1 = simply_evaluate(moves11,len1,max_move1,1,moves11[0]);
			pos1 = g_pos;

			val2 = simply_evaluate(moves12,len2,max_move2,1,moves12[0]);
			pos2 = g_pos;
			if(val1 >= val2)
				copy_array(moves11[pos1],des);
			if(val1 < val2)
				copy_array(moves12[pos2],des);
			return;
		}
		val1 = look_deep(moves11,len1,max_move1);
		pos1 = g_pos;
		val2 = look_deep(moves12,len2,max_move2);
		pos2 = g_pos;

		//		cout<<"pos1  "<<pos1<<"  pos2  "<<pos2<<endl;

		if(val1 < val2)
			copy_array(moves12[pos2],des);
		else
			copy_array(moves11[pos1],des);
	}
	else if(max_move1 > max_move2)
	{
		if(bearing_off == 1)
		{
			val1 = simply_evaluate(moves11,len1,max_move1,1,moves11[0]);
			copy_array(moves11[g_pos],des);
			return ;
		}
		val1 = look_deep(moves11,len1,max_move1);
		pos1 = g_pos;
		copy_array(moves11[pos1],des);
	}
	else
	{
		if(bearing_off == 1)
		{
			val2 = simply_evaluate(moves12,len2,max_move2,1,moves12[0]);
			copy_array(moves12[g_pos],des);
			return ;
		}
		val2 = look_deep(moves12,len2,max_move2);
		pos2 = g_pos;
		copy_array(moves12[pos2],des);
	}
}

int set_board(int moves[])
{
	int bar_val = 0;
	if(moves[0] != -1)
	{
		tmp[moves[0]] ++;
		if(tmp[moves[0]] == 0)
		{
			tmp[moves[0]] ++;
			nbar ++;
		}
		tmp[moves[1]] --;
	}
	if(moves[2] != -1)
	{
		tmp[moves[2]] ++;
		if(tmp[moves[2]] == 0)
		{
			tmp[moves[2]] ++;
			nbar ++;
		}
		tmp[moves[3]] --;
	}
	return bar_val;
}

void put_board()
{
	int x;
	for(x=1;x<=24;x++)
		cout << tmp[x] << " ";
	cout << endl;
}

double look_deep(int moves[][5],int sz,int max_move)
{
	int pos,x,y,z,tmp_bar,val1,len1,val2,len2,ret_pos;
	double res1,res2,final,sum,ret_val,first_config,value;
	int init_val,a,val,amove[5];

	ret_val=-infinity*1.0;
	reset_board();
	set_bar_var();
	set_board(moves[0]);
	init_val = evaluate(tmp);

	ret_pos = 0;

	for(x=0;x<sz;x++)
	{
		if(moves[x][4] < max_move)
			continue;

		//		cout<<moves[x][1]<<"  "<<moves[x][0]<<endl;
		//		cout<<moves[x][3]<<"  "<<moves[x][2]<<endl;
		reset_board();
		set_bar_var();
		val = set_board(moves[x]);
		nbar += val;
		//		put_board();
		first_config = evaluate(tmp);
		//		cout << " Value " << first_config << endl; 

		sum = 0;
		g_pos = -1;
		for(y=1;y<=6;y++)
		{
			for(z=y;z<=6;z++)
			{
				reset_board();
				set_bar_var();
				val = set_board(moves[x]);
				nbar += val;

				for(a=1;a<=24;a++)
					tmp[a] = -tmp[a];
				reverse_array();
				swap_bar_var();
				bearing_off = check_bearing();

				if(pbar>=1)
				{
					play_bar(y,z,amove,-1,moves[x]);
					set_board(amove);
					final = evaluate(tmp);
				}	
				else
				{
					val1 = give_states(y,z,moves_alice1,-1,moves[x]);
					len1 = sz;
					reset_board();
					set_bar_var();
					val = set_board(moves[x]);
					nbar += val;

					for(a=1;a<=24;a++)
						tmp[a] = -tmp[a];
					reverse_array();
					swap_bar_var();
					val2 = give_states(z,y,moves_alice2,-1,moves[x]);
					len2 = sz;
					if(val1 == val2)
					{
						res1 = simply_evaluate(moves_alice1,len1,val1,-1,moves[x]);
						res2 = simply_evaluate(moves_alice2,len2,val2,-1,moves[x]);
						final = res2;
						if(res1 > res2)
							final = res1;
					}
					else if(val1 < val2)
						final = simply_evaluate(moves_alice1,len1,val1,-1,moves[x]);
					else
						final = simply_evaluate(moves_alice2,len2,val2,-1,moves[x]);
				}
				if(y == z)
					sum += (final/36);
				else
					sum += (final/18);
			}
		}
		//		cout << "sum "<<sum <<endl;
		value = 0.5*first_config - 0.5*sum;
		//		cout << "final value  "<<value<<"  ret_val  "<<ret_val<<endl;
		if(ret_val < value)
		{
			ret_val = value;
			ret_pos = x;
		}
		else if(ret_val == value)
		{
			val1 = first_config;
			if(val1 > init_val)
				ret_pos = x;
		}
		//		cout << "ret_val  "<<ret_val <<endl;
	}
	g_pos = ret_pos;
	return ret_val;
}

int check_config(int pos,int die)
{
	int x = pos - die;
	int prev;
	if(bearing_off == 1)
	{
		if(pos > 25)
		{
			prev = pos - die;
			for(x=19;x<prev && tmp[x]<=0;x++);
			if(x<prev)
				return 0;
		}
		if(pos<=24)
		{
			tmp[pos] ++;
			if(tmp[pos] < 1)
			{
				tmp[pos] = 1;
				was_opponent = 1;
			}
		}
		tmp[pos - die] --;
		return 1;
	}
	if(pos <= 24 && tmp[pos] >= -1)
	{
		tmp[pos] ++;
		if(tmp[pos] < 1)
		{
			tmp[pos] = 1;
			was_opponent = 1;
		}
		tmp[x] --;
		return 1;
	}
	return 0;
}

int give_states(int die1,int die2,int moves[][5],int factor,int prev_move[])
{
	int g_fl,fl1,fl2,x,move1,pos,y,move2,a;
	g_fl = 0;
	st = 0;
	for(x=1;x<=24;x++)
	{
		move1 = -1;
		fl1 = 0;
		was_opponent = 0;
		reset_board();
		set_bar_var();
		if(factor == -1)
		{
			set_board(prev_move);
			for(a=1;a<=24;a++)
				tmp[a] = -tmp[a];
			reverse_array();
			swap_bar_var();
		}
		if(tmp[x] < 1)
			continue;
		pos = x + die1;
		fl1 = check_config(pos,die1);
		if(fl1 == 1)
		{
			move1 = pos;
			if(pos >24)
				move1 = 0;
		}
		if(g_fl == 2 && fl1 != 1)
			continue;
		for(y=1;y<=24;y++)
		{
			move2 = -1;
			fl2 = 0;
			was_opponent = 0;
			if(tmp[y] < 1)
				continue;
			pos = y + die2;
			fl2 = check_config(pos,die2);
			if(fl2 == 1)
			{
				move2 = pos;
				if(pos >24)
					move2 = 0;
			}
			if(fl2 == 1)
			{
				tmp[y] ++;
				if(move1 != 0)
				{
				tmp[pos] --;
				if(was_opponent)
					tmp[pos] --;
				}
			}
			if(g_fl <= fl1+fl2)
			{
				g_fl = fl1+fl2;
				//				cout << "st is  "<<st<<endl;
				moves[st][0] = move1;
				moves[st][1] = x;
				moves[st][2] = move2;
				moves[st][3] = y;
				moves[st++][4] = g_fl;
			}
		}
	}
	return g_fl;
}
