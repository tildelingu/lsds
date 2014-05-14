#include "libi0/stdio.h"
#include "libi0/stddef.h"
#include "libi0/stdint.h"

#define N (10000000) // training set size
#define M (100) // testing set size
#define C (3) // number of class
#define K (2) // K in K-NN
#define PSIZE (N/3)
#define N_LONG_PAGE 512
#define STDOUT (0x100000208)

#define DEBUG

// training set
standalone long x[N]; // 4 dimension points
standalone long y[N];
standalone long z[N];
standalone long w[N];
standalone long l[N]; // label

// testing set
standalone long xt[M]; // 4 dimension points
standalone long yt[M];
standalone long zt[M];
standalone long wt[M];
standalone long lt[M]; // true label
standalone long nn[M*K]; // the k nearest neighbour for each point in test set
standalone long dist[M*K];
standalone long label_predict[M*K];
standalone long label_aggregate[M]; // aggregated label for each test data, label in {0,1}
standalone long count[C];
// standalone long trigger[N];
// opt
// Note: each trigger's content should not > N_LONG_PAGE Bytes
standalone long trigger[(N/PSIZE+1)*N_LONG_PAGE];
standalone long trigger_t[(M/PSIZE+1)*N_LONG_PAGE];

#define distance_sq(x1, y1, z1, w1, x2, y2, z2, w2) \
(((x1) - (x2)) * ((x1) - (x2))) + \
(((y1) - (y2)) * ((y1) - (y2))) + \
(((z1) - (z2)) * ((z1) - (z2))) + \
(((w1) - (w2)) * ((w1) - (w2)))

long distance_sq_0(long x1, long y1, long z1, long w1, long x2, long y2, long z2, long w2)
{
    long xd, yd, zd, wd;
    xd = (x1 - x2);
    yd = (y1 - y2);
    zd = (z1 - z2);
    wd = (w1 - w2);
    return xd * xd + yd * yd + zd * zd + wd * wd;
}

void try()
{
    long i;

    output_char('h');
    output_char('e');
    output_char('l');
    output_char('l');
    output_char('o');
    output_char(' ');
    output_char('w');
    output_char('o');
    output_char('r');
    output_char('d');
    output_char(10);

    commit;
}

// input train data into x[],y[],z[],w[],l[]
void init_train_stdin(long start, long end)
{
    long i;
    long t;

    for(i = 0; i < start; i = i + 1)
    {
            t = input_q();
            t = input_q();
            t = input_q();
            t = input_q();
            t = input_q(); // label
    }

    output_char('t');
    output_char('r');
    output_char('a');
    output_char('i');
    output_char('n');
    output_char(' ');
    output_char('d');
    output_char('a');
    output_char('t');
    output_char('a');
    output_char(C_n);

    for(i = start; i < end; i = i + 1)
    {
        x[i] = input_q();
#ifdef DEBUG
        output_q(x[i]);
        output_char(' ');
#endif
        y[i] = input_q();
#ifdef DEBUG
        output_q(y[i]);
        output_char(' ');
#endif
        z[i] = input_q();
#ifdef DEBUG
        output_q(z[i]);
        output_char(' ');
#endif
        w[i] = input_q();
#ifdef DEBUG
        output_q(w[i]);
        output_char(' ');
#endif
        l[i] = input_q();
#ifdef DEBUG
        output_q(l[i]);
        output_char(C_n);
#endif

    }

    for(i = start; i < end; i = i + 1) {
        trigger[i/PSIZE*N_LONG_PAGE] = 1;
    }

    commit;
}

void init_test_stdin(long start, long end)
{
    long i;
    long t;
    output_char('a');
    for(i = 0; i < N/PSIZE * N_LONG_PAGE; i = i + N_LONG_PAGE)
    {
        if(trigger[i] != 1)
        {
            abort;
        }
    }    
    for(i = 0; i < start; i = i + 1)
    {
            t = input_q();
            t = input_q();
            t = input_q();
            t = input_q();
	    t = input_q();
    }

    output_char('t');
    output_char('e');
    output_char('s');
    output_char('t');
    output_char(' ');
    output_char('d');
    output_char('a');
    output_char('t');
    output_char('a');
    output_char(' ');
    output_char(C_n);

    for(i = start; i < end; i = i + 1)
    {
        xt[i] = input_q();
#ifdef DEBUG
        output_q(xt[i]);
        output_char(' ');
#endif
        yt[i] = input_q();
#ifdef DEBUG
        output_q(yt[i]);
        output_char(' ');
#endif
        zt[i] = input_q();
#ifdef DEBUG
        output_q(zt[i]);
        output_char(' ');
#endif
        wt[i] = input_q();
#ifdef DEBUG
        output_q(wt[i]);
        output_char(' ');
#endif
        lt[i] = input_q(); // the label for test set is not read
#ifdef DEBUG
        output_q(lt[i]);
        output_char(C_n);
#endif
    }
    for(i = start; i < end; i = i + 1) {
        trigger_t[i/PSIZE*N_LONG_PAGE] = 1;
    }

    commitd;
}

// aggregate the neighbour info into a prediction label, label start from 0,1,2,...
// output the result
void aggregate()
{
    long i,j;
    long maxid, maxvalue;

    for(i = 0; i < M/PSIZE * N_LONG_PAGE; i = i + N_LONG_PAGE)
    {
        if(trigger_t[i] != 2)
        {
            abort;
        }
    }

    output_char(C_n);
    output_char('a');
    output_char('g');
    output_char('g');
    output_char(' ');
    output_char('o');
    output_char('u');
    output_char('t');
    output_char('p');
    output_char('u');
    output_char('t');
    output_char(C_n);


    for(i = 0; i < M; i = i + 1)
    {
	output_char('i');
        output_char('=');
        output_q(i);
        output_char(' ');

	
	for (j = 0; j < C; j = j + 1)
	{
	    count[j] = 0;
	}
	for(j = 0; j < K; j = j + 1)
	{
	    count[label_predict[i*K + j]] = count[label_predict[i * K + j]] + 1;
	}
	maxvalue = -1;
	for (j = 0; j < C; j = j + 1)
	{
	    if (count[j] > maxvalue)
	    {
		maxvalue = count[j];
		maxid = j;
	    }
	}
	label_aggregate[i] = maxid;
	// output the attribute and result
	output_q(xt[i]);
	output_char(' ');
        output_q(yt[i]);
        output_char(' ');
        output_q(zt[i]);
        output_char(' ');
        output_q(wt[i]);
        output_char(' ');
        output_char('p');
        output_char('r');
        output_char('e');
        output_char('d');
        output_char(':');
        output_q(label_aggregate[i]);
        output_char(' ');
        output_char('t');
        output_char('r');
        output_char('u');
        output_char('e');
        output_char(':');
        output_q(lt[i]);
        output_char(C_n);
    }
    commitd;
}

// find nearest neighbour info for each of the data point
void predict(long start, long end)
{
    long i,j,p;
    long i_max,d_max; // index and distance for current farest point in dist_temp[K]
    long d; // temp distance
    long x_temp, y_temp, z_temp, w_temp; // current test data point
    long dist_temp[K]; // current test shortest distance
    long nn_temp[K]; // current test shortest index
    long label_temp[K]; // current test shorest neighbour label

    for(i = start; i < end; i = i + 1)
    {
	x_temp = xt[i];
	y_temp = yt[i];
	z_temp = zt[i];
	w_temp = wt[i];

	// initial the nearest neighbour to be the first K points in dataset
	j = 0;
	d = distance_sq(x_temp, y_temp, z_temp, w_temp, x[j], y[j], z[j], w[j]);
	dist_temp[j] = d;
	label_temp[j] = l[j];
	i_max = j;
	d_max = d;
	for(j = 1; j < K; j = j + 1)
	{
	    d = distance_sq(x_temp, y_temp, z_temp, w_temp, x[j], y[j], z[j], w[j]);
	    dist_temp[j] = d;
	    nn_temp[j] = j;
	    label_temp[j] = l[j];
	    if(d > d_max) // update farest point if needed
	    {
		d_max = d;
		i_max = j;
	    }
	}

	// traverse the whole train set
	// update the nearest neighbour
	for(j = K; j < N; j = j + 1)
	{
	    d = distance_sq(x_temp, y_temp, z_temp, w_temp, x[j], y[j], z[j], w[j]);
	    // replace the farest point
	    if(d < d_max)
	    {
		dist_temp[i_max] = d;
		nn_temp[i_max] = j;
		label_temp[i_max] = l[j];
		d_max = d;
		// update d_max and i_max
		for(p = 0; p < K; p = p + 1)
		{
		    if(dist_temp[p]>d_max)
		    {
			d_max = dist_temp[p];
			i_max = p;
		    }
		}
	    }
	}

        output_char('p');
        output_char('r');
        output_char('e');
        output_char('d');
        output_char(' ');
        output_q(i);
        output_char(C_n);


	// copy back the results: dist_temp, nn_temp, label_temp
	for(j = 0; j < K; j = j + 1)
	{
	    label_predict[i*K+j] = label_temp[j];
	    nn[i*K+j] = nn_temp[j];
	    dist[i*K+j] = dist_temp[j];
	}
    }
    trigger_t[start/PSIZE*N_LONG_PAGE] = 2;
   // trigger[start/PSIZE*N_LONG_PAGE] = 2;
    commit;
}

void on_init_finished()
{
    long i;
    for(i = 0; i < N/PSIZE * N_LONG_PAGE; i = i + N_LONG_PAGE)
    {
        if(trigger[i] != 1)
        {
            abort;
        }
    }
    for(i = 0; i < M/PSIZE * N_LONG_PAGE; i = i + N_LONG_PAGE)
    {
        if(trigger_t[i] != 1)
        {
            abort;
        }
    }
    trigger[0] = 3;
    trigger_t[0] = 3;

    output_char(C_n);
    output_char('s');
    output_char('t');
    output_char('a');
    output_char('r');
    output_char('t');
    output_char(C_n);

    // for each point in test set, 
    for(i = 0; i < M; i = i + PSIZE)
    {
	output_char(C_n);
	output_char('p');
	output_char('r');
	output_char('e');
	output_char('d');
	output_char('t');
        output_char(i);
	output_char(C_n);

        trigger_t[i/PSIZE*N_LONG_PAGE] = 0;
	// find nn[], dist[], label_predict[] for each test dataset
	runner 
	    predict(i, i + PSIZE)
	    using x[0,, N], y[0,, N], z[0,, N], w[0,, N], l[0,, N], xt[i,, i+PSIZE], yt[i,, i+PSIZE], zt[i,, i+PSIZE], wt[i,, i+PSIZE], nn[i*K,, i*K+PSIZE*K], dist[i*K,, i*K+PSIZE*K], label_predict[i*K,, i*K+PSIZE*K], trigger_t[i/PSIZE*N_LONG_PAGE, , (i/PSIZE+1)*N_LONG_PAGE]
	;
    }
        output_char(C_n);
        output_char('a');
        output_char('g');
        output_char('g');
        output_char('r');
        output_char('e');
        output_char('g');
        output_char(C_n);

    // output the label for each testcase
    runner
	aggregate()
	using xt[0,,M], yt[0,, M], zt[0,, M], wt[0,, M], lt[0,, M], label_predict[0,,M*K], label_aggregate[0,,M], count[0,,C]
	watching trigger_t[0,, (M/PSIZE+1)*N_LONG_PAGE]
	;
    commitd;
}

// to input all data into memory
void init0_stdin()
{
    long i;
    for(i = 0; i < N; i = i + PSIZE)
    {
        trigger[i/PSIZE*N_LONG_PAGE] = 0;
    }

    runner init_train_stdin(0,N)
            using x[0,, N], y[0,, N], z[0,, N], w[0,, N], l[0,, N], trigger[0/PSIZE*N_LONG_PAGE, , (N/PSIZE+1)*N_LONG_PAGE]
    ;

    runner init_test_stdin(0,M)
            using xt[0,, M], yt[0,, M], zt[0,, M], wt[0,, M], lt[0,, M], trigger_t[0/PSIZE*N_LONG_PAGE, , (M/PSIZE+1)*N_LONG_PAGE]
		, trigger[0/PSIZE*N_LONG_PAGE, , (N/PSIZE+1)*N_LONG_PAGE]
	    watching trigger[0/PSIZE*N_LONG_PAGE, , (N/PSIZE+1)*N_LONG_PAGE] 
    ;

    runner on_init_finished()
        using x[0, , N], y[0, , N], z[0, , N], w[0, , N], l[0,, N], trigger[0, , (N/PSIZE+1)*N_LONG_PAGE], xt[0,, M], yt[0,, M], zt[0,, M], wt[0,, M], lt[0,, M], trigger_t[0,, (M/PSIZE+1)*N_LONG_PAGE], nn[0,, M*K], dist[0,, M*K], label_predict[0,,M*K]
        watching trigger[0, , (N/PSIZE+1)*N_LONG_PAGE], trigger_t[0, , (M/PSIZE+1)*N_LONG_PAGE]
    ;

    commit;
}


void main()
{
    runner init0_stdin()
            using trigger[0, , (N/PSIZE+1)*N_LONG_PAGE], trigger_t[0, , (M/PSIZE+1)*N_LONG_PAGE];
    commit;
}

