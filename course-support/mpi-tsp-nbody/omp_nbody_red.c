/* 文件名：  omp_nbody_red.c
 *
 * 功能：    利用OpenMP来并行化处理一个2维n体问题。该版本使用简化算法， 
 *           且每个线程使用一个数组来存储本地计算出来的作用力，然后再把这
 *           些作用力累加到一个共享数组里去。每个parallel for下面的块使用
 *           块划分策略，但计算力的那个循环例外，它使用的是循环划分。
 *
 * 编译命令：gcc -g -Wall -fopenmp -o omp_nbody_red omp_nbody_red.c -lm
 *           NO_OUTPUT用来关闭计时等结果的异常输出
 *           DEBUG宏用来得到更多的输出结果
 *
 * 使用方法： ./omp_nbody_red <number of threads> <number of particles>
 *              <number of timesteps>  <size of timestep> 
 *              <output frequency> <g|i>
 *              'g': 使用一个随机数生成器来产生初始化条件
 *              'i': 从标准输入设备读入初始化条件
 *            对自动生成的数据来说，把时间步设为0.01似乎比较合理
 *
 * 输入参数： 若从控制台指定为“g”，则无需输入参数。
 *           若从控制台指定为“i”，则应初始化每一个粒子的初始位置和初始速度
 * 输出参数： 若输出频率为k，则为每一k时间步时各粒子的位置和速度
 *
 * 作用力：  粒子i对k施加的作用力为
 *    -G m_i m_k (s_i - s_k)/|s_i - s_k|^3
 *
 * 这里m_j为粒子j的质量，s_j是粒子j当前位置的速度（时间t）
 * G是万有引力常数（见下）。 
 *
 * 注意到粒子i给粒子k的作用力= -(粒子k给粒子i的作用力)，因此我们大概可以
 * 将作用力的计算量消减一半。
 *
 * 结合：  我们使用了欧拉方法：
 *
 *    v_i(t+1) = v_i(t) + h v'_i(t)
 *    s_i(t+1) = s_i(t) + h v_i(t)
 *
 * 这里v_i(u)是第i个粒子在时间u的速度，s_i(u)是其位置
 *
 * 教材对应位置：第6.1.6节 (教材第191页往后)
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <omp.h>

#define DIM 2  /* 二维系统  */
#define X 0    /* x下标分量 */
#define Y 1    /* y下标分量 */

const double G = 6.673e-11;  /* 万有引力常数       */
                             /* 单位 m^3/(kg*s^2)  */

typedef double vect_t[DIM];  /* 存储位置等矢量类型 */

struct particle_s {
   double m;  /* 质量 */
   vect_t s;  /* 位置 */
   vect_t v;  /* 速度 */
};

void Usage(char* prog_name);
void Get_args(int argc, char* argv[], int* thread_count_p, int* n_p, 
      int* n_steps_p, double* delta_t_p, int* output_freq_p, char* g_i_p);
void Get_init_cond(struct particle_s curr[], int n);
void Gen_init_cond(struct particle_s curr[], int n);
void Output_state(double time, struct particle_s curr[], int n);
void Compute_force(int part, vect_t forces[], struct particle_s curr[], 
      int n);
void Update_part(int part, vect_t forces[], struct particle_s curr[], 
      int n, double delta_t);

/*--------------------------------------------------------------------*/
int main(int argc, char* argv[]) {
   int n;                      /* 粒子个数                     */
   int n_steps;                /* 时间步数目                   */
   int step;                   /* 当前步                       */
   int part;                   /* 当前粒子                     */
   int output_freq;            /* 输出频率                     */
   double delta_t;             /* 时间步长度                   */
   double t;                   /* 当前时间                     */
   struct particle_s* curr;    /* 系统当前状态                 */
   vect_t* forces;             /* 作用在每个粒子上的力         */
   int thread_count;           /* 线程数量                     */
   char g_i;                   /* 是自动生成还是人工输入初始值 */
   double start, finish;       /* 计时用变量                   */
   vect_t* loc_forces;         /* 本地作用力                   */

   Get_args(argc, argv, &thread_count, &n, &n_steps, &delta_t, 
         &output_freq, &g_i);
   curr = malloc(n*sizeof(struct particle_s));
   forces = malloc(n*sizeof(vect_t));
   loc_forces = malloc(thread_count*n*sizeof(vect_t));
   if (g_i == 'i')
      Get_init_cond(curr, n);
   else
      Gen_init_cond(curr, n);

   start = omp_get_wtime();
#  ifndef NO_OUTPUT
   Output_state(0, curr, n);
#  endif
#  pragma omp parallel num_threads(thread_count) default(none) \
      shared(curr,forces,thread_count,delta_t,n,n_steps, \
            output_freq,loc_forces) \
      private(step, part, t)
   {
      int my_rank = omp_get_thread_num();
      int thread;

      for (step = 1; step <= n_steps; step++) {
         t = step*delta_t;
//       memset(loc_forces + my_rank*n, 0, n*sizeof(vect_t));
#        pragma omp for
         for (part = 0; part < thread_count*n; part++)
            loc_forces[part][X] = loc_forces[part][Y] = 0.0;
#        ifdef DEBUG
#        pragma omp single
         {
            printf("Step %d, after memset loc_forces = \n", step);
            for (part = 0; part < thread_count*n; part++)
               printf("%d %e %e\n", part, loc_forces[part][X], 
                  loc_forces[part][Y]);
            printf("\n");
         }
#        endif
         /* Particle n-1 will have all forces computed after call to
          * Compute_force(n-2, . . .) */
#        pragma omp for schedule(static,1)
         for (part = 0; part < n-1; part++)
            Compute_force(part, loc_forces + my_rank*n, curr, n);
#        pragma omp for 
         for (part = 0; part < n; part++) {
            forces[part][X] = forces[part][Y] = 0.0;
            for (thread = 0; thread < thread_count; thread++) {
               forces[part][X] += loc_forces[thread*n + part][X];
               forces[part][Y] += loc_forces[thread*n + part][Y];
            }
         }
#        pragma omp for
         for (part = 0; part < n; part++)
            Update_part(part, forces, curr, n, delta_t);
#        ifndef NO_OUTPUT
         if (step % output_freq == 0) {
#           pragma omp single
            Output_state(t, curr, n);
         }
#        endif
      }  /* for step */
   }  /* pragma omp parallel */
   finish = omp_get_wtime();
   printf("Elapsed time = %e seconds\n", finish-start);

   free(curr);
   free(forces);
   free(loc_forces);
   return 0;
}  /* main */

/*---------------------------------------------------------------------
 * 函数名： Usage
 * 功能：   打印命令行指令并退出
 * 输入参数：   
 *    prog_name:  命令行输进去的程序名称
 */
void Usage(char* prog_name) {
   fprintf(stderr, "usage: %s <number of threads> <number of particles>\n",
         prog_name);
   fprintf(stderr, "   <number of timesteps>  <size of timestep>\n");
   fprintf(stderr, "   <output frequency> <g|i>\n");
   fprintf(stderr, "   'g': program should generate init conds\n");
   fprintf(stderr, "   'i': program should get init conds from stdin\n");
    
   exit(0);
}  /* Usage */


/*---------------------------------------------------------------------
 * 函数名：   Get_args
 * 功能：     Get command line args
 * 输入 args:
 *      argc:          命令行参数个数
 *      argv:          命令行参数
 * 输出参数：
 *    thread_count_p:  指向thread_count的指针。
 *    n_p:             指向n的指针, n表示粒子个数
 *    n_steps_p:       指向n_steps的指针, n_steps表示时间步个数
 *    delta_t_p:       指向delta_t的指针, 表示每个时间步长度
 *    output_freq_p:   指向output_freq的指针，output_freq表示将要输出
 *                     的时间步间隔
 *    g_i_p:           指向char，用以表示初始条件应当由系统自动生成，
 *                     还是从标准输入设备人工读入。
 */
void Get_args(int argc, char* argv[], int* thread_count_p, int* n_p, 
      int* n_steps_p, double* delta_t_p, int* output_freq_p, 
      char* g_i_p) {
   if (argc != 7) Usage(argv[0]);
   *thread_count_p = strtol(argv[1], NULL, 10);
   *n_p = strtol(argv[2], NULL, 10);
   *n_steps_p = strtol(argv[3], NULL, 10);
   *delta_t_p = strtod(argv[4], NULL);
   *output_freq_p = strtol(argv[5], NULL, 10);
   *g_i_p = argv[6][0];

   if (*thread_count_p <= 0 || *n_p <= 0 || *n_steps_p < 0 ||
       *delta_t_p <= 0) Usage(argv[0]);
   if (*g_i_p != 'g' && *g_i_p != 'i') Usage(argv[0]);

#  ifdef DEBUG
   printf("thread_count = %d\n", *thread_count_p);
   printf("n = %d\n", *n_p);
   printf("n_steps = %d\n", *n_steps_p);
   printf("delta_t = %e\n", *delta_t_p);
   printf("output_freq = %d\n", *output_freq_p);
   printf("g_i = %c\n", *g_i_p);
#  endif
}  /* Get_args */

/*---------------------------------------------------------------------
 * 函数名：   Get_init_cond
 * 功能：     读入初始条件：每个粒子的质量、位置和速度
 * 输入参数：  
 *    n:      粒子个数
 * 输出参数：
 *    curr:   n个结构体数组。每个结构体存储一个粒子的质量（标量）
 *            位置（矢量）和速度（矢量）
 */
void Get_init_cond(struct particle_s curr[], int n) {
   int part;

   printf("For each particle, enter (in order):\n");
   printf("   its mass, its x-coord, its y-coord, ");
   printf("its x-velocity, its y-velocity\n");
   for (part = 0; part < n; part++) {
      scanf("%lf", &curr[part].m);
      scanf("%lf", &curr[part].s[X]);
      scanf("%lf", &curr[part].s[Y]);
      scanf("%lf", &curr[part].v[X]);
      scanf("%lf", &curr[part].v[Y]);
   }
}  /* Get_init_cond */

/*---------------------------------------------------------------------
 * 函数名：   Gen_init_cond
 * 功能：     生成初始条件：每个粒子的质量、位置和速度
 * 输入参数： 
 *    n:      粒子个数
 * 输出参数：
 *    curr:   n个元素的结构体数组。每个结构体存储一个粒子的质量
 *            （标量）、位置（矢量）和速度（矢量）
 *
 * 注意：     初始状态置所有的粒子，沿x轴非负方向等距离分布
 *            每个粒子具有相同的质量，速度统一沿y轴方向，但有 
 *            些沿y轴正向，有些沿y轴负向。
 */
void Gen_init_cond(struct particle_s curr[], int n) {
   int part;
   double mass = 5.0e24;
   double gap = 1.0e5;
   double speed = 3.0e4;

   srandom(1);
   for (part = 0; part < n; part++) {
      curr[part].m = mass;
      curr[part].s[X] = part*gap;
      curr[part].s[Y] = 0.0;
      curr[part].v[X] = 0.0;
//    if (random()/((double) RAND_MAX) >= 0.5)
      if (part % 2 == 0)
         curr[part].v[Y] = speed;
      else
         curr[part].v[Y] = -speed;
   }
}  /* Gen_init_cond */

/*---------------------------------------------------------------------
 * 函数名：   Output_state
 * 功能：     打印输出系统的当前状态
 * 输入参数：
 *    curr:   具有n个元素的数组。curr[i]存储第i个粒子的状态
 *            （质量、位置和速度）
 *    n:     粒子数目
 */
void Output_state(double time, struct particle_s curr[], int n) {
   int part;
   printf("%.2f\n", time);
   for (part = 0; part < n; part++) {
//    printf("%.3e ", curr[part].m);
      printf("%3d %10.3e ", part, curr[part].s[X]);
      printf("  %10.3e ", curr[part].s[Y]);
      printf("  %10.3e ", curr[part].v[X]);
      printf("  %10.3e\n", curr[part].v[Y]);
   }
   printf("\n");
}  /* Output_state */


/*---------------------------------------------------------------------
 * 函数名：   Compute_force
 * 功能：     计算粒子上的总作用力。利用对称性原理（i施加给k的作用力）=
 *            -（k施加给i的作用力），同时也计算另外一些粒子的部分作用力
 * 输入参数：   
 *    part:   要计算总作用力的那部分粒子 
 *    curr:   当前系统状态：curr[i] 存储第i个粒子的质量、位置和速度
 *    n:      粒子数目
 * 输出参数：
 *     forces: force[i] 存储作用在第i个粒子的总作用力
 *
 * 注意： 该函数使用万有引力定律，因此粒子i施加给粒子k的作用力为
 *
 *    m_i m_k (s_k - s_i)/|s_k - s_i|^2
 *
 * 这里，m_j 是第j个粒子质量。s_k 是其位置向量（t时刻）
 */
void Compute_force(int part, vect_t forces[], struct particle_s curr[], 
      int n) {
   int k;
   double mg; 
   vect_t f_part_k;
   double len, len_3, fact;

#  ifdef DEBUG
   printf("Current total force on particle %d = (%.3e, %.3e)\n",
         part, forces[part][X], forces[part][Y]);
#  endif
   for (k = part+1; k < n; k++) {
      /* Compute force on part due to k */
      f_part_k[X] = curr[part].s[X] - curr[k].s[X];
      f_part_k[Y] = curr[part].s[Y] - curr[k].s[Y];
      len = sqrt(f_part_k[X]*f_part_k[X] + f_part_k[Y]*f_part_k[Y]);
      len_3 = len*len*len;
      mg = -G*curr[part].m*curr[k].m;
      fact = mg/len_3;
      f_part_k[X] *= fact;
      f_part_k[Y] *= fact;
#     ifdef DEBUG
      printf("Force on particle %d due to particle %d = (%.3e, %.3e)\n",
            part, k, f_part_k[X], f_part_k[Y]);
#     endif

      /* Add force into total forces */
      forces[part][X] += f_part_k[X];
      forces[part][Y] += f_part_k[Y];
      forces[k][X] -= f_part_k[X];
      forces[k][Y] -= f_part_k[Y];
   }
}  /* Compute_force */


/*---------------------------------------------------------------------
 * 函数名：    Update_part
 * 功能：      更新部分粒子的速度和位置
 * 输入参数：
 *    part:    将要更新的那部分粒子
 *    forces:  forces[i] 存储作用在第i个粒子的总作用力
 *    n:       粒子数目
 *
 * 输入/输出参数：
 *    curr:    curr[i]存储第i个粒子的质量、位置和速度
 *
 * 注意：  该版本使用欧拉方法更新速度和位置
 */
void Update_part(int part, vect_t forces[], struct particle_s curr[], 
      int n, double delta_t) {
   double fact = delta_t/curr[part].m;

#  ifdef DEBUG
   printf("Before update of %d:\n", part);
   printf("   Position  = (%.3e, %.3e)\n", curr[part].s[X], curr[part].s[Y]);
   printf("   Velocity  = (%.3e, %.3e)\n", curr[part].v[X], curr[part].v[Y]);
   printf("   Net force = (%.3e, %.3e)\n", forces[part][X], forces[part][Y]);
#  endif
   curr[part].s[X] += delta_t * curr[part].v[X];
   curr[part].s[Y] += delta_t * curr[part].v[Y];
   curr[part].v[X] += fact * forces[part][X];
   curr[part].v[Y] += fact * forces[part][Y];
#  ifdef DEBUG
   printf("Position of %d = (%.3e, %.3e), Velocity = (%.3e,%.3e)\n",
         part, curr[part].s[X], curr[part].s[Y],
               curr[part].v[X], curr[part].v[Y]);
#  endif
// curr[part].s[X] += delta_t * curr[part].v[X];
// curr[part].s[Y] += delta_t * curr[part].v[Y];
}  /* Update_part */
