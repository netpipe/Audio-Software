#define _XOPEN_SOURCE_EXTENDED 

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <signal.h>
#include <locale.h>
#include <curses.h>
#include <pulse/simple.h>
#include <pulse/error.h>
#include <fftw3.h>

struct sigaction old_sigint;
bool run;

int framesPerSecond = 30;
double upperFrequency = 3520.0; // A7
double gain = 1.0;

void onSigInt()
{
	// reset SIGINT.
	sigaction(SIGINT, &old_sigint, NULL);
	
	// tell main loop to exit.
	run = false;
}

// hanning window.
float windowFunction(int n, int N)
{
	return 0.5f * (1.0f - cosf(2.0f * M_PI * n / (N - 1.0f)));
}

void printUsage()
{
	printf("PulseAudio Spectrum Analyzer\n");
	printf("\nUsage:\n");
	printf("pasa [options] pulseaudio-device\n");
	printf("\nOptions:\n");
	printf("-r <n>\tframes per second (default 30)\n");
	printf("-f <n>\tmaximum frequency (default 3520)\n");
	printf("-g <n>\tgain (i.e. a scaling factor for the bars, default 1.0)\n");
}

void calculateBars(fftw_complex* fft, int fftSize, int* bars, int numBars)
{
	// todo: use the float-point value and implement proper interpolation.
	double barWidthD = upperFrequency / (framesPerSecond * numBars);
	int barWidth = (int)ceil(barWidthD);

	double scale = 2.0 / fftSize * gain;
	
	// interpolate bars.
	int i = 0;
	for(int bar = 0; bar < numBars; bar++)
	{
		// get average.
		double power = 0.0;
		for(int j = 0; j < barWidth && i < fftSize; i++, j++)
		{
			double re = fft[i][0] * scale;
			double im = fft[i][1] * scale;
			power += re * re + im * im; // abs(c)
		}
		power *= (1.0 / barWidth); // average.
		if(power < 1e-15) power = 1e-15; // prevent overflows.

		// compute decibels.
		int dB = LINES + (int)(10.0 * log10(power));
		if(dB > LINES) dB = LINES;
		if(dB < 0) dB = 0;

		// set bar.
		bars[bar] = dB;
	}
}

int main(int argc, char* argv[])
{
	static const pa_sample_spec ss =
	{
		.format = PA_SAMPLE_FLOAT32LE,
		.rate = 44100,
		.channels = 2
	};

	// parse command line arguments.
	int c;
	while((c = getopt(argc, argv, "r:f:g:")) != -1)
	{
		switch(c)
		{
			case 'r':
				framesPerSecond = atoi(optarg);
				break;

			case 'f':
				upperFrequency = atof(optarg);
				break;

			case 'g':
				gain = atof(optarg);
				break;

			case '?':
				printUsage();
				return 1;

			default:
				abort();
		}
	}

	// open record device
	int error;
	pa_simple *s = pa_simple_new(NULL, "pasa", PA_STREAM_RECORD, optind < argc ? argv[optind] : NULL, "pasa", &ss, NULL, NULL, &error);

	// check error
	if(!s)
	{
		fprintf(stderr, "pa_simple_new() failed: %s\n", pa_strerror(error));
		return 1;
	}

	// input buffer.
	const int size = ss.rate / framesPerSecond;
	float window[size];
	float buffer[ss.channels * size];

	// compute window.
	for(int n = 0; n < size; n++)
		window[n] = windowFunction(n, size);

	// replace SIGINT handler.
	struct sigaction sigIntAction;
	memset(&sigIntAction, 0, sizeof(sigIntAction));
	sigIntAction.sa_handler = &onSigInt;
	sigaction(SIGINT, &sigIntAction, &old_sigint);
	
	// fftw setup
	double *in = (double*)fftw_malloc(sizeof(double) * size);
	fftw_complex *out = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * size);
	fftw_plan plan = fftw_plan_dft_r2c_1d(size, in, out, FFTW_MEASURE);

	run = true;

	// ncurses setup.
	setlocale(LC_ALL, "");
	initscr();
	//start_color();
	curs_set(0);

	// record loop
	while(run)
	{
		int barsL[COLS / 2];
		int barsR[COLS / 2];

		// read from device.
		if (pa_simple_read(s, buffer, sizeof(buffer), &error) < 0)
		{
			pa_simple_free(s);
			fprintf(stderr, "pa_simple_read() failed: %s\n", pa_strerror(error));
			return 1;
		}

		// left input.
		for(int i = 0; i < size; i++) in[i] = (double)(window[i] * buffer[i * 2]);
		fftw_execute(plan);
		calculateBars(out, size, barsL, COLS / 2);
		
		// right input.
		for(int i = 0; i < size; i++) in[i] = (double)(window[i] * buffer[i * 2 + 1]);
		fftw_execute(plan);
		calculateBars(out, size, barsR, COLS / 2);

		// draw bars.
		erase();
		
		// draw left
		for(int i = 0; i < COLS / 2; i++)
		{
			move(LINES - barsL[i], i);
			vline(ACS_VLINE, barsL[i]);
		}

		// draw right.
		for(int i = 0; i < COLS / 2; i++)
		{
			move(LINES - barsR[i], COLS - 1 - i);
			vline(ACS_VLINE, barsR[i]);
		}

		// draw to screen.
		refresh();
	}

	// clean up fftw
	fftw_destroy_plan(plan);
	fftw_free(in);
	fftw_free(out);

	// clean up pulseaudio
	pa_simple_free(s);

	// close ncurses
	endwin();

	return 0;
}
