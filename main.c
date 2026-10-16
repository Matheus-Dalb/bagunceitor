/*

Para compilar de dentro do VS Code:

Windows/Linux: CTRL+SHIFT+B (Terminal -> Run Build Task)
macOS: COMMAND+SHIFT+B

Para executar de dentro do VS Code:

Executar normalmente: CTRL+F5 (Run -> Run Without Debugging)
Debugar: F5 (Run -> Start Debugging)

Pelo terminal:

Windows: mingw32-make
Linux/macOS: make

Para executar:

./bagunceitor [arquivo com a imagem de entrada]

*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>  // Para usar strings
#include <time.h>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#include <stb_image.h>
#include <stb_image_write.h>

// Um pixel Pixel (24 bits)
typedef struct {
  unsigned char r, g, b;
} Pixel;

// Uma imagem Pixel
typedef struct {
  int width, height;  // largura, altura
  int channels;       // qtd de canais (geralmente 3, RGB)
  Pixel* pixels;
} Img;

// As 2 imagens
Img in, out;

// Protótipos
void load(char* name, Img* pic);
void confusao(Img *imagem);
void confusaoInversa(Img *imagem);
void difusao(Img *imagem);
void difusaoInversa(Img *imagem);


int main(int argc, char* argv[]) {
  if (argc < 2) {
    printf("bagunceitor [origem]\n");
    exit(1);
  }

  // Carrega a imagem original
  load(argv[1], &in);

  // Exibe as dimensões na tela, para conferência
  printf("Origem   : %s %d x %d\n", argv[1], in.width, in.height);

  printf("Processando...\n");

  // Cria imagem de saída
  int tam = in.width * in.height;
  out = in;
  out.pixels = malloc(tam * sizeof(Pixel));

  if (out.pixels == NULL) {
    printf("Erro ao alocar memoria!\n");
    stbi_image_free(in.pixels);
    return 1;
  }

  // Copia os pixels da imagem original para a imagem de saída
  memcpy(out.pixels, in.pixels, tam * sizeof(Pixel));

  // Converte para interpretar como matrizes
  Pixel(*pin)[in.width] = (Pixel(*)[in.width])in.pixels;
  Pixel(*pout)[in.width] = (Pixel(*)[in.width])out.pixels;

  //
  // Implementação dos algoritmos
  //

  // Aplica confusão e difusão
  confusao(&out);
  difusao(&out);

  // Grava a imagem transformada
  stbi_write_png("saida.png", out.width, out.height, 3, pout, 0);

  // Desfaz as operações para recuperar a imagem original
  difusaoInversa(&out);
  confusaoInversa(&out);
 // Confere se a imagem recuperada é igual à original
  int iguais = 1;

for (int i = 0; i < tam; i++) {
  if (in.pixels[i].r != out.pixels[i].r ||
      in.pixels[i].g != out.pixels[i].g ||
      in.pixels[i].b != out.pixels[i].b) {
    iguais = 0;
    break;
  }
}

if (iguais) {
  printf("A imagem recuperada esta exatamente igual.\n");
} else {
  printf("A imagem recuperada esta diferente da original\n");
}
  // Grava a imagem recuperada para teste
  stbi_write_png("recuperada.png", out.width, out.height, 3, pout, 0);

  free(in.pixels);
  free(out.pixels);
}

  //OS NUMEROS 40, 20, E 206, NÃO FORAM ESCOLHIDOS ALEATORIAMENTE, EU ESCOLHI POIS MINHA DATA DE NASCIMENTO É 04/02/2006, CURIOSAMENTE.
  void confusao(Img *imagem) { 
    int tam = imagem->width * imagem->height;

    for (int i = 0; i < tam; i++) {
        imagem->pixels[i].r =
            (imagem->pixels[i].r + 40) % 256;

        imagem->pixels[i].g =
            (imagem->pixels[i].g + 20) % 256;

        imagem->pixels[i].b =
            (imagem->pixels[i].b + 206) % 256;
    }
}

void confusaoInversa(Img *imagem) {
    int tam = imagem->width * imagem->height;

    for (int i = 0; i < tam; i++) {
        imagem->pixels[i].r =
            (imagem->pixels[i].r - 40 + 256) % 256;

        imagem->pixels[i].g =
            (imagem->pixels[i].g - 20 + 256) % 256;

        imagem->pixels[i].b =
            (imagem->pixels[i].b - 206 + 256) % 256;
    }
}

void difusao(Img *imagem) {
    int largura = imagem->width;
    int altura = imagem->height;

    Pixel *linhaTemp = malloc(largura * sizeof(Pixel));

    if (linhaTemp == NULL) {
        printf("erro na memoria\n");
        return;
    }

    for (int i = 0; i < altura; i++) {
        int deslocamento = (i + 1) % largura;

        // copia a linha original
        for (int j = 0; j < largura; j++) {
            linhaTemp[j] = imagem->pixels[i * largura + j];
        }

        // muda os pixeis de posição para as novas posições
        for (int j = 0; j < largura; j++) {
            int novaPosicao = (j + deslocamento) % largura;

            imagem->pixels[i * largura + novaPosicao] = linhaTemp[j];
        }
    }

    free(linhaTemp);
}

void difusaoInversa(Img *imagem) {
    int largura = imagem->width;
    int altura = imagem->height;

    Pixel *linhaTemp = malloc(largura * sizeof(Pixel));

    if (linhaTemp == NULL) {
        printf("erro na memoria\n");
        return;
    }

    for (int i = 0; i < altura; i++) {
        int deslocamento = (i + 1) % largura;

        // copia a linha
        for (int j = 0; j < largura; j++) {
            linhaTemp[j] = imagem->pixels[i * largura + j];
        }

        // muda os pixeis de posição para as originais
        for (int j = 0; j < largura; j++) {
            int novaPosicao = (j - deslocamento + largura) % largura;

            imagem->pixels[i * largura + novaPosicao] = linhaTemp[j];
        }
    }

    free(linhaTemp);
}

void load(char* name, Img* pic) {
  pic->pixels =
      stbi_load(name, &pic->width, &pic->height,
          &pic->channels, 3);//mudei para garantir que a imagem seja carregada em RGB
  if (!pic->pixels) {
    printf("Erro de leitura: %s\n", stbi_failure_reason());
    exit(1);
  }
  printf("Load: %d x %d x %d\n", pic->width, pic->height, pic->channels);
  // Exibe um bloco de 8 x 8 pixels em hexadecimal (teste)
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      printf("[%02X %02X %02X] ", pic->pixels[i].r, pic->pixels[i].g,
             pic->pixels[i].b);
    }
    printf("\n");
  }
}