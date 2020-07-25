#include <stdio.h>
#include <string.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <sys/timeb.h>
#include <unistd.h>
#include <time.h>

struct STAR {
    float x, y, z;
};

void nearestNeighbor(Uint32 * pix, Uint32 * dest, int w1, int h1, int w2, int h2) {
    float x_ratio = w1 / (float) w2;
    float y_ratio = h1 / (float) h2;

    float px, py;

    int i, j;
    for(i=0; i<h2; i++) {
        for(j=0; j<w2; j++) {
            px = floor(j * x_ratio);
            py = floor(i * y_ratio);

            dest[(i * w2) + j] = pix[(int)((py*w1) + px)];
        }
    }
}

void spawnStar(struct STAR * star_array, int i, float x, float y, float z) {
    float rand1 = (float)(rand() % (1024)) / 1024.0;
    float rand2 = (float)(rand() % (1024)) / 1024.0;
    float rand3 = (float)(rand() % (1024)) / 1024.0;
                
    star_array[i].x = x;
    star_array[i].y = y;
    star_array[i].z = z;
}

int main(int argc, char * argv[]) {
    srand(time(0));
    
    // SDL writable surface
    Uint32 * pixels = (Uint32*) malloc(sizeof(Uint32) * 240 * 240); //original surface
    Uint32 * pixlarge = (Uint32*) malloc(sizeof(Uint32) * 960 * 960); //output surface after scaling

    SDL_Window * window;
    SDL_Init(SDL_INIT_VIDEO);

    window = SDL_CreateWindow("starfield", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 960, 960, SDL_WINDOW_OPENGL);
    if(window == NULL) {
        printf("Can't create window: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Renderer * renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_Texture * texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STATIC, 960, 960);

    //for delta calculations
    struct timeb start, end;
    ftime(&start);

    int numofstars;
    float speed;

    printf("input num of stars: "); scanf("%d", &numofstars);
    
    //workaround to catch newline character from user input above
    char tmp;
    scanf("%c", &tmp);

    printf("input stars travel speed: "); scanf("%f", &speed);
    printf("%d %f\n", numofstars, speed);

    struct STAR * stars = (struct STAR*) malloc(sizeof(struct STAR) * numofstars);
    
    int i = 0;
    for(i=0; i<numofstars; i++) {
        float rand1 = (float)(rand() % (1024)) / 1024.0;
        float rand2 = (float)(rand() % (1024)) / 1024.0;
        float rand3 = (float)(rand() % (1024)) / 1024.0;
        spawnStar(stars, i, (rand1 * 2.0) - 1.0, (rand2 * 2.0) - 1.0, rand3 * 5.0);
    }
    
    //main loop
    while(!0) {
        //calculate delta
        ftime(&end);
        float delta = (1000.0 * (end.time - start.time) + (end.millitm - start.millitm));
        ftime(&start);

        //set all pixels to black
        memset(pixels, 0xff000000, sizeof(Uint32) * 240 * 240);
        memset(pixlarge, 0xff000000, sizeof(Uint32) * 960 * 960);

        for(i=0; i<numofstars; i++) {
            //bring current star towards the camera
            stars[i].z -= speed * delta;

            //if current star Z location is behind the camera, re-initialize it on other location
            if(stars[i].z <= 0.0) {
                float rand1 = (float)(rand() % (1024)) / 1024.0;
                float rand2 = (float)(rand() % (1024)) / 1024.0;
                float rand3 = (float)(rand() % (1024)) / 1024.0;
                spawnStar(stars, i, (rand1 * 2.0) - 1.0, (rand2 * 2.0) - 1.0, rand3 * 5.0);
            }

            float displayx = 240.0 * (stars[i].x+stars[i].z) / 2.0 * stars[i].z;
            float displayy = 240.0 * (stars[i].y+stars[i].z) / 2.0 * stars[i].z;

            //if current star is out of screen space, re-initialize it on other location
            if((displayx < 0.0 || displayx >= 240.0)||(displayy < 0.0 || displayy >= 240.0)) {
                float rand1 = (float)(rand() % (1024)) / 1024.0;
                float rand2 = (float)(rand() % (1024)) / 1024.0;
                float rand3 = (float)(rand() % (1024)) / 1024.0;
                spawnStar(stars, i, (rand1 * 2.0) - 1.0, (rand2 * 2.0) - 1.0, rand3 * 5.0);;
            } else {
                //otherwise show the current star in white color
                //
                //pixel format:
                //    AA BB GG RR
                //0 x FF FF FF FF
                pixels[((int)displayy * 240) + (int)displayx] = 0xffffffff;
            }            
        }

        //scale pixels to 960x960 using nearest neighbor and store it on pixlarge
        nearestNeighbor(pixels, pixlarge, 240, 240, 960, 960);

        //update texture and present to screen
        SDL_UpdateTexture(texture, NULL, pixlarge, 960 * sizeof(Uint32));
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
    }

    printf("quit\n");
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}