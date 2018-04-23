
/*
*********��ǰ��3.25___����ѹ������ͼ�Ĵ洢�ṹ������㼯���߼�
��һ����������ɫ���ϱ� ��������ɫ��ÿһ����ɫ������ĳ�ͻ����ʼ������
*********��ǰ��3.31___���岢��ʼ����ɫ���ϱ�������ɫ�����Խ����ȷ
��һ��������������ɫ�����ɳ�ͻ��ÿ����ɫ����һ������һ��Ԫ�ؼ�¼��ͻ������
                             ��ά�������ÿ����¼����������Ƿ��ͻ��
							 ����ṹ����ô�ң�
							 ����ɫ���ϸ���������ṹ��


*/


#include <stdio.h>
#include <time.h>
#include <windows.h>

/***********************************���峣��*******************************************/
#define EDGE 736
#define NODE 125
#define BEST 5
/***********************************���庯��*******************************************/
void Initialize_Graph();
void Initialize_Colorset_table();
void Initialize_Adjacent_table();
void Initialize_Conflict_table();
void Search_Node();
void Init_f();//��ʼ����ʧ����
void Find_Move();//�ҵ���ǰ���ſ��н�
void Make_Move();
void Print_solution();//��ӡ���

/***********************************�������ݽṹ*******************************************/
int Graph[NODE+1][NODE+1]= { 0 };//���������ͼ 
int Color_Set[BEST + 1][NODE + 1] = { 0 };//��ɫ���ϱ����
int Adjacent_Color_table[NODE + 1][BEST + 1] = { 0 };//������ɫ��,ÿһ�еĵ�һ����Ԫ�ؼ�¼�Լ�����ɫ
int conflict_location[NODE + 1] = { 0 };//��¼��ͻ����ڳ�ͻ���е�λ��
int conflict_table[NODE + 1] = { 0 };//��¼��ͻ���
int best_f[3] = { 0 };//���浱ǰ��ʹ��ʧ������С������н� [0]=�ƶ����ţ�[1]=������ɫ��[2]=�ƶ������ɫ
int delta_f = 0;//��ǰ��ʧ�����ı仯�� �Ƴ����ϵ����ڱ���-���뼯�ϵ����ڱ��� Խ��Խ��
int best_delta_f_ever = 0;//��ʷ��õ���ʧ����������
int Tabu_table[NODE + 1][BEST + 1] = { 0 };//���ɱ�
int Current_f = 0;//��ʼ��ʧ������ֵ
int itter = 0;//��ǰ��������


/***********************************����ؼ�����*******************************************/
int main(void)
{
	Initialize_Graph();//��ʼ������ͼѹ������
	Initialize_Colorset_table();//��ʼ����ɫ���Ͼ���
	Initialize_Adjacent_table();//��ʼ����ɫ���ڱ�
	Initialize_Conflict_table();//��ʼ����ͻ��ͳ�ͻλ�ñ�
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

void Initialize_Graph()//��ʼ������ͼ����
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
	srand((unsigned)time(NULL)); //��ʱ�����֣�ÿ�β����������һ��
	int count = 1;
	int rand_color;
	for (; count <= NODE; count++)
	{
		rand_color = rand() % BEST + 1;
		Color_Set[rand_color][count] = 1;
		Adjacent_Color_table[count][0] = rand_color;//˳���¼�³�ͻ���н�����ɫ
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
			conflict_table[current_location] = i;//��ͻ�������ͻ��
			conflict_location[i] = current_location;//��ͻλ�ü�¼
			current_location += 1;
		}
	}
}

void Init_f()//��ʼ����ʧ����
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
	int flag = 0;//���������ף���ͬ��ý��ҵ��Ĵ�����ÿһ�������µ����Ž�ʱ����
	for (; i <= NODE; i++)
	{
		int color = 1;
		for (; color <= BEST; color++)
		{
			own_color = Adjacent_Color_table[i][0];
			if (own_color == color)
			{
				continue;//�����Ƶ��Լ�����ɫ
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

void Make_Move()//ʵʩ�ƶ�
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


		//�ڳ�ͻ����ɾ������ͻ�Ľ��
		if (Graph[move_node][current_adjacent] == 1 && Adjacent_Color_table[current_adjacent][0] == former_color && Adjacent_Color_table[current_adjacent][former_color] == 0)
		{
			location = conflict_location[current_adjacent];
			conflict_table[location] = 0;
			conflict_table[0] -= 1;
		}

		//�ڳ�ͻ��������µĳ�ͻ���
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