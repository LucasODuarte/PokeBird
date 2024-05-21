#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <time.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
 

//-------Variáveis Globais
const long largura_tela = 360;
const long altura_tela = 640;
const int FPS = 60;
enum TECLAS {CIMA, SPACE, ENTER};
ALLEGRO_SAMPLE *theme = NULL;
ALLEGRO_SAMPLE_INSTANCE *musicatema=NULL;

//-------Inicialização de Funções e Structs
int colisao(int ax1, int ay1, int ax2, int ay2, int bx1, int by1, int bx2, int by2)
{
    if(ax1 > bx2) return 0;
    if(ax2 < bx1) return 0;
    if(ay1 > by2) return 0;
    if(ay2 < by1) return 0;

    return 1;
}

typedef struct PokemonPrincipal{
   int x;
   int y;
   int vidas;
   int colisao_x;
   int colisao_y;
   int pontos;
   ALLEGRO_BITMAP* imagem;
   ALLEGRO_BITMAP* imagem_1;
}PokemonPrincipal;

typedef struct PokemonInimigo{
   int x;
   int y;
   int velocidade;
   int colisao_x;
   int colisao_y;
   int ativo;
   ALLEGRO_BITMAP* imagem;
   ALLEGRO_BITMAP* imagem_1;
}PokemonInimigo;

void initPokemonInimigo(PokemonInimigo* pokemon){
    pokemon->x=270;
    pokemon->y=305;
    pokemon->colisao_x=pokemon->x+70;
    pokemon->colisao_y=pokemon->y+53;
    pokemon->velocidade=4;
    pokemon->imagem=al_load_bitmap("Snorlax_deitado.png");
    pokemon->imagem_1 = al_create_sub_bitmap(pokemon->imagem, 5, 50, 72, 53);
}

void initPokemonInimigo2(PokemonInimigo* pokemon){
    pokemon->x=270;
    pokemon->y=180;
    pokemon->colisao_x=pokemon->x+55;
    pokemon->colisao_y=pokemon->y+44;
    pokemon->velocidade=5;
    pokemon->imagem=al_load_bitmap("pidgeot.png");
    pokemon->imagem_1 = al_create_sub_bitmap(pokemon->imagem, 6, 4, 55, 44);
}

void initPokemonPrincipal(PokemonPrincipal* pokemon){
   pokemon->x=15;
   pokemon->y=324;
   pokemon->vidas=1;
   pokemon->colisao_x=pokemon->x+23;
   pokemon->colisao_y=pokemon->y+24;
   pokemon->pontos=0;
   pokemon->imagem = al_load_bitmap("zard.png");
   pokemon->imagem_1 = al_create_sub_bitmap(pokemon->imagem, 1, 1, 41, 25);
}

//tempo de liberação pro pokemon andar
void liberaPokemonInimigo(PokemonInimigo* pokemon){
        if(pokemon->ativo==0){
            if(rand() % 5 == 0){
                pokemon->x=280;
                pokemon->y=305;
                pokemon->ativo=1;
            }
        } 
}

//tempo de liberação pro pokemon andar
void liberaPokemonInimigo2(PokemonInimigo* pokemon){
        if(pokemon->ativo==0){
            if(rand() % 10 == 0){
                pokemon->x=270;
                pokemon->y=180;
                pokemon->ativo=1;
            }
        } 
}



void desenhaPokePrincipal(PokemonPrincipal* pokemon){
    al_draw_bitmap(pokemon->imagem_1, pokemon->x, pokemon->y, 0);
}

void desenhaPokeInimigo(PokemonInimigo* pokemon){
   al_draw_bitmap(pokemon->imagem_1, pokemon->x, pokemon->y, 0);
}

void desenhaFimDeJogo(ALLEGRO_BITMAP* telafimdejogo){
   al_draw_bitmap(telafimdejogo, 0, 0, 0);
}

void desenhaChao(ALLEGRO_BITMAP* chao){
    al_draw_bitmap(chao, 0, 344, 0);
}

void desenhaCeu(ALLEGRO_BITMAP* ceu){
    static long movimento=0;
    al_draw_bitmap(ceu, movimento, 0, 0);
    movimento--;
    if(movimento<-720){
        movimento=0;
    }
}

//quando morre, seta no x inicial para nao bugar o menu
void atualizaPokemonInimigoInicio(PokemonInimigo* pokemon){
        if(pokemon->ativo == 1){
            pokemon->x = 280;
            pokemon->ativo=0;
        }   
}

//faz o inimigo avançar, saindo da tela volta pro inicio
void atualizaPokemonInimigo(PokemonInimigo* pokemon){
        if(pokemon->ativo == 1){
            pokemon->x -= pokemon->velocidade;

            if (pokemon->x<-70){
                pokemon->ativo=0;
            }
        }   
}

//altura voo maximo
void atualizaAlturaPokemonPrincipal(PokemonPrincipal* pokemon){
    if(pokemon->y<324)
        pokemon->y += 4;
}

//n usamos
void avancaPokemonInimigo(PokemonInimigo* pokemon){
    pokemon->x -= pokemon->velocidade;
    if (pokemon->x<-70)
        pokemon->x=350;
    
}

void pulaPokemonPrincipal(PokemonPrincipal* pokemon){
    pokemon->y -= 7;
    if(pokemon->y < 0)
        pokemon->y = 0;
}

void resetaPokemonInicioMorte(PokemonPrincipal* pokemon){
    pokemon->x=15;
    pokemon->y=324;
}

int main()
{
    if(!al_init()) {
      fprintf(stderr, "failed to initialize allegro!\n");
      return -1;
   }

   if(!al_init_image_addon())
    {
        printf("couldn't initialize image addon\n");
        return 1;
    }

    //-------Variáveis do Jogo
    ALLEGRO_EVENT_QUEUE *fila_eventos = NULL;
    ALLEGRO_TIMER *timer = NULL;
    ALLEGRO_BITMAP* chao = al_load_bitmap("chao.png");
    ALLEGRO_BITMAP* ceu1 = al_load_bitmap("ceu3.png");
    ALLEGRO_BITMAP* telafimdejogo = al_load_bitmap("fimdejogo.png");
    ALLEGRO_BITMAP* ceu = al_create_sub_bitmap(ceu1, 0, 0, 1080, 350);
    int done=1;
    int desenha=1;
    int fimdejogo=0;
    int tocarmusicatema=0;
    int teclas[] = {0 ,0, 0};

    //-------Inicialização de Objetos
    PokemonPrincipal mander65;
    PokemonInimigo snorlax;
    PokemonInimigo pidgeot;

    //-------Inicialização do Display
    ALLEGRO_DISPLAY *display;

    display = al_create_display(largura_tela, altura_tela);

    if(!display) {
      fprintf(stderr, "failed to create display!\n");
      return -1;
   }

    //-------Funções iniciais
    srand(time(NULL));
    initPokemonPrincipal(&mander65);
    initPokemonInimigo(&snorlax);
    initPokemonInimigo2(&pidgeot);

    //-------Inicialização de Addons e Instalações
    al_install_keyboard();
    al_install_audio();
    al_init_acodec_addon();
    al_reserve_samples(1);

    //-------Criação da Fila e Demais Dispositivos
    fila_eventos = al_create_event_queue();
    timer = al_create_timer(1.0/FPS);
    theme = al_load_sample("musica.wav");
    musicatema=al_create_sample_instance(theme);
    al_attach_sample_instance_to_mixer(musicatema, al_get_default_mixer());
    al_set_sample_instance_playmode(musicatema, ALLEGRO_PLAYMODE_LOOP);
    al_set_sample_instance_gain(musicatema, 0.6);

    //-------Registro de Sources
    al_register_event_source(fila_eventos, al_get_display_event_source(display));
    al_register_event_source(fila_eventos, al_get_timer_event_source(timer));
    al_register_event_source(fila_eventos, al_get_keyboard_event_source());

    //-------Loop Inicial
    al_start_timer(timer);
    int pressionado=1;
    while(done!=0){
       ALLEGRO_EVENT evento;
       al_wait_for_event(fila_eventos, &evento);
 
       //-------Logica do Jogo
       if(evento.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
           done = 0;
       } else if(evento.type == ALLEGRO_EVENT_KEY_DOWN){

           switch (evento.keyboard.keycode){
            case ALLEGRO_KEY_ESCAPE:
               done=0;
               break;
            case ALLEGRO_KEY_UP:
               teclas[CIMA]=1;
               break;
            case ALLEGRO_KEY_SPACE:
                    teclas[SPACE]=1;
               break;
            case ALLEGRO_KEY_ENTER:
                teclas[ENTER]=1;
                break;
           }

       }else if(evento.type == ALLEGRO_EVENT_KEY_UP){

           switch (evento.keyboard.keycode){
            case ALLEGRO_KEY_UP:
               teclas[CIMA]=0;
               break;
            case ALLEGRO_KEY_SPACE:
               teclas[SPACE]=0;
               break;
            case ALLEGRO_KEY_ENTER:
                teclas[ENTER]=0;
                break;
           }

       } else if (evento.type == ALLEGRO_EVENT_TIMER){
            desenha=1;
            if(teclas[CIMA])
                pulaPokemonPrincipal(&mander65);
                
                
            if(teclas[SPACE])
                pulaPokemonPrincipal(&mander65);
                
            if(fimdejogo==1){
                desenha=0;
                al_stop_sample_instance(musicatema);
                resetaPokemonInicioMorte(&mander65);
                if(teclas[ENTER]==1){
                    fimdejogo=0; 
                    desenha=1;
                }
                atualizaPokemonInimigoInicio(&snorlax);
                atualizaPokemonInimigoInicio(&pidgeot);
            }else{
                al_play_sample_instance(musicatema);
                desenha=1;
                liberaPokemonInimigo(&snorlax);
                liberaPokemonInimigo2(&pidgeot);
                atualizaPokemonInimigo(&snorlax);
                atualizaPokemonInimigo(&pidgeot);
                atualizaAlturaPokemonPrincipal(&mander65);

                if(colisao(mander65.x, mander65.colisao_y, mander65.colisao_x, mander65.y, snorlax.x, snorlax.y, snorlax.colisao_x, snorlax.colisao_y) == 1){
                    fimdejogo=1;
                    desenha=0;
                }

                if(colisao(mander65.x, mander65.y, mander65.colisao_x, mander65.colisao_y, pidgeot.x, pidgeot.y, pidgeot.colisao_x, pidgeot.colisao_y) == 1){
                    fimdejogo=1;
                    desenha=0;
                }
            }
            
       }

       //-------Desenho da Tela
       if(desenha==1 && al_is_event_queue_empty(fila_eventos)){
        desenhaCeu(ceu);
        desenhaChao(chao);
        desenhaPokePrincipal(&mander65);
        desenhaPokeInimigo(&snorlax);
        desenhaPokeInimigo(&pidgeot);
       }else if (fimdejogo==1){
            desenhaFimDeJogo(telafimdejogo);
       }
       al_flip_display();
    }
    


    al_destroy_display(display);
    al_destroy_event_queue(fila_eventos);
    al_destroy_bitmap(mander65.imagem_1);
    al_destroy_bitmap(mander65.imagem);
    al_destroy_bitmap(pidgeot.imagem_1);
    al_destroy_bitmap(pidgeot.imagem);
    al_destroy_bitmap(snorlax.imagem_1);
    al_destroy_bitmap(snorlax.imagem);
    al_destroy_bitmap(telafimdejogo);
    al_destroy_bitmap(chao);
    al_destroy_bitmap(ceu);
    al_destroy_sample(theme);
    al_destroy_sample_instance(musicatema);

    
    return 0;
}