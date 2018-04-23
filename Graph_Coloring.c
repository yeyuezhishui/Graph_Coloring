
/*
*********当前：3.25___定义压缩矩阵图的存储结构，载入点集，边集
下一步：定义颜色集合表 、相邻颜色表、每一个颜色集合里的冲突表，初始化他们
*********当前：3.31___定义并初始化颜色集合表，相邻颜色表，调试结果正确
下一步：根据相邻颜色表生成冲突表：每个颜色集合一个表？第一个元素记录冲突个数？
                             二维数组结点表？每个记录和其他结点是否冲突？
							 领域结构域怎么找？
							 用颜色集合更好找领域结构。


*/


#include <stdio.h>
#include <time.h>
#include <windows.h>

/***********************************定义常量*******************************************/
#define EDGE 736
#define NODE 125
#define BEST 5
/***********************************定义函数*******************************************/
void Initialize_Graph();
void Initialize_Colorset_table();
void Initialize_Adjacent_table();
void Initialize_Conflict_table();
void Search_Node();
void Init_f();//初始化损失函数
void Find_Move();//找到当前最优可行解
void Make_Move();
void Print_solution();//打印结果

/***********************************定义数据结构*******************************************/
int Graph[NODE+1][NODE+1]= { 0 };//矩阵存无向图 
int Color_Set[BEST + 1][NODE + 1] = { 0 };//颜色集合表矩阵
int Adjacent_Color_table[NODE + 1][BEST + 1] = { 0 };//相邻颜色表,每一行的第一个列元素记录自己的颜色
int conflict_location[NODE + 1] = { 0 };//记录冲突结点在冲突表中的位置
int conflict_table[NODE + 1] = { 0 };//记录冲突结点
int best_f[3] = { 0 };//保存当前能使损失函数最小的最可行解 [0]=移动结点号，[1]=本来颜色，[2]=移动后的颜色
int delta_f = 0;//当前损失函数的变化量 移出集合的相邻边数-移入集合的相邻边数 越大越好
int best_delta_f_ever = 0;//历史最好的损失函数减少量
int Tabu_table[NODE + 1][BEST + 1] = { 0 };//禁忌表
int Current_f = 0;//初始损失函数的值
int itter = 0;//当前迭代次数


/***********************************定义关键变量*******************************************/
int main(void)
{
	Initialize_Graph();//初始化无向图压缩矩阵
	Initialize_Colorset_table();//初始化颜色集合矩阵
	Initialize_Adjacent_table();//初始化颜色相邻表
	Initialize_Conflict_table();//初始化冲突表和冲突位置表
	Init_f();
	while (1)
	{
		Find_Move();
		Make_Move();
		if (Current_f == 0)
		{
			break;
		}
		itter++;
	}
	Print_solution();
	printf("%d", conflict_table[1]);
	system("pause");
	return 0;
}

void Initialize_Graph()//初始化无向图矩阵
{
	FILE *fp = NULL;
	fp = fopen("col.txt", "r");
	char ch;
	int i = 0;
	int j = 0;
	int count = 0;
	for (; count<EDGE; count++)
	{
		fread(&ch, sizeof(char), 1, fp);
		fscanf(fp, "%d", &i);
		fscanf(fp, "%d", &j);
		fgetc(fp);
		Graph[i][j] = 1;
		Graph[j][i] = 1;
		i = 0;
		j = 0;
	}
	fclose(fp);

}

void Initialize_Colorset_table()
{
	srand((unsigned)time(NULL)); //用时间做种，每次产生随机数不一样
	int count = 1;
	int rand_color;
	for (; count <= NODE; count++)
	{
		rand_color = rand() % BEST + 1;
		Color_Set[rand_color][count] = 1;
		Adjacent_Color_table[count][0] = rand_color;//顺便记录下冲突表中结点的颜色
	}
}

void Initialize_Adjacent_table()
{
	int i = 1;
	int j = 1;
	int color;
	for (; i <= NODE; i++)
	{
		for (j = 1; j <= NODE; j++)
		{
			if (Graph[i][j] == 1)
			{
				color = Adjacent_Color_table[j][0];
				Adjacent_Color_table[i][color] += 1;
			}
		}
	}
	
}

void Initialize_Conflict_table()
{
	int i = 1;
	int j = 1;
	int own_color = 0;
	int current_location = 1;
	for (; i <= NODE; i++)
	{
		own_color = Adjacent_Color_table[i][0];
		if (Adjacent_Color_table[i][own_color] != 0)
		{
			conflict_table[0] += 1;
			conflict_table[current_location] = i;//冲突结点放入冲突表
			conflict_location[i] = current_location;//冲突位置记录
			current_location += 1;
		}
	}
}

void Init_f()//初始化损失函数
{
	int i = 1;
	int own_color = 0;
	for (; i <= NODE; i++)
	{
		own_color = Adjacent_Color_table[i][0];
		Current_f += Adjacent_Color_table[i][own_color];
	}
	Current_f = Current_f / 2;
}

void Find_Move()
{
	int i = 1;
	int color = 1;
	int current_delta_f = 0;
	int best_delta_f = 0;
	int own_color = 0;
	int flag = 0;//狗熊掰玉米，相同最好解找到的次数，每一次有最新的最优解时更新
	for (; i <= NODE; i++)
	{
		int color = 1;
		for (; color <= BEST; color++)
		{
			own_color = Adjacent_Color_table[i][0];
			if (own_color == color)
			{
				continue;//不会移到自己的颜色
			}
			current_delta_f = Adjacent_Color_table[i][own_color] - Adjacent_Color_table[i][color];
			if (Tabu_table[i][color] > itter && current_delta_f <= best_delta_f_ever)
			{
				break;
			}
			if (current_delta_f > best_delta_f)
			{
				flag = 0;
				best_delta_f = current_delta_f;
				best_f[0] = i;
				best_f[1] = own_color;
				best_f[2] = color;
			}
			if (current_delta_f > best_delta_f_ever)
			{
				best_delta_f_ever = current_delta_f;
			}
			if (current_delta_f == best_delta_f)
			{
				flag += 1;
				if (rand() % flag + 1 <= 1)
				{
					best_delta_f = current_delta_f;
					best_f[0] = i;
					best_f[1] = own_color;
					best_f[2] = color;
				}
			}
		}
	}
	delta_f = best_delta_f;
}

void Make_Move()//实施移动
{
	int move_node = best_f[0];
	int former_color = best_f[1];
	int new_color = best_f[2];
	int current_adjacent = 1;
	int location = 0;
	Color_Set[former_color][move_node] = 0;
	Color_Set[new_color][move_node] = 1;
	Adjacent_Color_table[move_node][0] = new_color;
	for (; current_adjacent <= NODE; current_adjacent++)
	{
		if (current_adjacent == move_node)
		{
			continue;
		}
		if (Graph[move_node][current_adjacent] == 1)
		{
			Adjacent_Color_table[current_adjacent][former_color] -= 1;
			Adjacent_Color_table[current_adjacent][new_color] += 1;
		}


		//在冲突表中删除不冲突的结点
		if (Graph[move_node][current_adjacent] == 1 && Adjacent_Color_table[current_adjacent][0] == former_color && Adjacent_Color_table[current_adjacent][former_color] == 0)
		{
			location = conflict_location[current_adjacent];
			conflict_table[location] = 0;
			conflict_table[0] -= 1;
		}

		//在冲突表中添加新的冲突结点
		int blank_location = 1;
		if (Graph[move_node][current_adjacent] == 1 && Adjacent_Color_table[current_adjacent][0] == new_color && Adjacent_Color_table[current_adjacent][new_color] == 1)
		{
			conflict_table[0] += 1;
			while (1)
			{
				if (conflict_table[blank_location] == 0)
				{
					conflict_table[blank_location] = current_adjacent;
					conflict_location[current_adjacent] = blank_location;
					break;
				}
				else
				{
					blank_location += 1;
				}
			}
		}
	}
	Current_f -= delta_f;
	Tabu_table[move_node][former_color] = itter + Current_f + rand() % 10 + 1;
}

void Print_solution()
{
	int i = 1;
	for (; i <= NODE; i++)
	{
		printf("%d get color:%d\n", i, Adjacent_Color_table[i][0]);
	}
}