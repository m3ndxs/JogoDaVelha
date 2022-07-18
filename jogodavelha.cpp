#include<GL/freeglut.h>
#include<iostream>
#include<stdlib.h>
#include<stdio.h>
#include<math.h>

#define PI 3.1415926535898

float x, y, ang;
bool check = true;
int vezJogador = 0;
int identificaJogadas[3][3]; //[linha][coluna]
bool fimJogo = false;

void desenhaTabuleiro();
void desenhaTexto(void *font, char *string);
void texto(void);
void teclado(unsigned char tecla, GLint x, GLint y);
void mouseClick(int button, int state, int mousex, int mousey);
void desenhaFormas(int mousex, int mousey, int formas);
void areaClick();
void percorreMatriz();
void defineVezJogador();
int defineJogadas();
void defineVencedor();
void linhaVencedorHorizontal(int y);
void linhaVencedorVertical(int x);
void linhaVencedorDiagonal(float y, float y1);
bool verificaTabuleiro();
void reiniciaJogo();


int main(int argc, char* argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode( GLUT_SINGLE | GLUT_RGB );
	glutInitWindowSize(640,480);
	glutInitWindowPosition(10, 10);	
	glutCreateWindow("Jogo da Velha");
	
	glutDisplayFunc(desenhaTabuleiro);	//chamada funcao de desenho
	glutMouseFunc(mouseClick);  //chamada funcao mouse
	glutKeyboardFunc(teclado);  //chamada funcao teclado
	
	glClearColor( 0, 0, 0, 1);
	glutMainLoop();		
		
	return 0;
}

void desenhaTabuleiro(){	
	glClear( GL_COLOR_BUFFER_BIT );	
	glColor3f(1,1,1);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, 640.0, 0.0, 480.0);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glLineWidth(4);
	
	glBegin(GL_LINES);
		//Vertical 1
		glVertex2f(230,350);
		glVertex2f(230,20);
		//Vertical 2
		glVertex2f(400,350);
		glVertex2f(400,20);
		//Horizontal 1
		glVertex2f(80,120);
		glVertex2f(560,120);
		//Horizontal 2
		glVertex2f(80,240);
		glVertex2f(560,240);
	glEnd();
	
	texto();  //chamada da função texto, para aparecer as escritas no display
	
	areaClick();	//definir a area de jogada, aonde o jogador pode clicar
	
	defineVencedor();	//desenha a linha de vencedor, seja horizontal, vertical ou diagonal
	
	glFlush();
}
void desenhaTexto(void *font, char *string){
		while(*string)
		glutStrokeCharacter(GLUT_STROKE_ROMAN,*string++);
}
void texto(void){   //desenhar os textos no display de jogo
	glPushMatrix();		
	    glColor3f(0,1,0);
	    glTranslatef(180,430,0);
	    glScalef(0.3, 0.3, 0.3); // diminui o tamanho do fonte
	    glLineWidth(4); // define a espessura da linha
	    desenhaTexto(GLUT_STROKE_ROMAN,"Jogo da Velha");
	glPopMatrix();
	glEnd();
	glFlush();
	
	glPushMatrix();
		glColor3f(1,1,1);
	    glTranslatef(460,5,0);
	    glScalef(0.12, 0.12, 0.12);
	    glLineWidth(1); 
	    desenhaTexto(GLUT_STROKE_ROMAN,"Leonardo Mendes");
	glPopMatrix();
	glEnd();
	glFlush();	
	
	glPushMatrix();
		glColor3f(1,1,1);
	    glTranslatef(40,5,0);
	    glScalef(0.12, 0.12, 0.12);
	    glLineWidth(1); 
	    desenhaTexto(GLUT_STROKE_ROMAN,"Pressione 'R' para reiniciar o jogo");
	glPopMatrix();
	glEnd();
	glFlush();	
}
void teclado(unsigned char tecla, GLint x, GLint y){   //função para usar o teclado
	switch(tecla){
		case'r':
		case'R':
			reiniciaJogo();	
			break;		
	}
	desenhaTabuleiro();
}
void mouseClick(int button, int state, int mousex, int mousey){		//função para usar o mouse
	if(fimJogo)
		return;
	if(state == GLUT_DOWN){
		if(button==GLUT_LEFT_BUTTON){	//identifica o clica com a parte esquerda do mouse
			check = true;		//check verifica se houve click ou não: true houve click, false não houve click;
        	x = mousex;
        	y = 440-mousey;
		}	
	}
    glutPostRedisplay();
}

void desenhaFormas(int mousex, int mousey, int formas){		//função para desenhar as formas do jogo, X ou O
	if(formas == 0)
		return;
			
	glPushMatrix();
		glLineWidth(4);
		
		if(check && formas == 1){    //desenha o X
			glColor3f(0,0,1);
			glBegin(GL_LINES);
				glVertex2f(mousex, mousey);
				glVertex2f(75 + mousex, 75 + mousey);
				glVertex2f(75 + mousex, mousey);
				glVertex2f(mousex, 75 + mousey);
			glEnd();
			glFlush();
		}
		else if(check && formas == 2){			//desenha o O
			glColor3f(1,0,0);
			glBegin(GL_LINE_LOOP);
			for (int i = 0; i < 360; i++) {
		        ang = (i * PI) / 180;
		        x = 35 + (cos(ang) * 45);
		        y = 40 + (sin(ang) * 45);
		        glVertex2f(x + mousex, y + mousey);
		    }				
			glEnd();
			glFlush();
		}
	glPopMatrix();
	defineVezJogador();  //verificar se é a vez do jogador X ou do jogador O
	glFlush();
}

//Uma matriz de quadrantes[3][3] para representar o tabuleiro
//[00] [01] [02]            x= horizontal 1 e 2
//[10] [11] [12]		    y = vertical  1 e 2
//[20] [21] [22]			

//Q1 = (x > 41.0 && x < 187.0) && (y >203 &&y < 311)	Q2 = (x > 192.0 && x < 355.0) && (y >203 &&y < 311)		Q3 = (x > 362.0 && x < 518.0) && (y >203 &&y < 311)
//Q4 = (x > 41.0 && x < 187.0) && (y >82.0 &&y < 198)	Q5 = (x > 192.0 && x < 355.0) && (y >82 &&y < 198)		Q6 = (x > 362.0 && x < 518.0) && (y >82 &&y < 198)
//Q7 = (x > 41.0 && x < 187.0) && (y >-18 &&y < 78)		Q8 = (x > 192.0 && x < 355.0) && (y >-18 &&y < 78)		Q9 = (x > 362.0 && x < 518.0) && (y >-18 &&y < 78)

void areaClick(){	//define o campo de jogada, ou seja o quadrante aonde o jogador clica
	if((x > 84.0 && x < 224.0) && (y >203 &&y < 311) && identificaJogadas[0][0] == 0){ //Q1		
		identificaJogadas[0][0] = defineJogadas();		
		vezJogador = identificaJogadas[0][0];		
	}
	else if((x > 232.0 && x < 395.0) && (y >203 &&y < 311) && identificaJogadas[0][1] == 0){ //Q2
		identificaJogadas[0][1] = defineJogadas();
		vezJogador = identificaJogadas[0][1];
	}
	else if((x > 403.0 && x < 588.0) && (y >203 &&y < 311) && identificaJogadas[0][2] == 0){  //Q3
		identificaJogadas[0][2] = defineJogadas();
		vezJogador = identificaJogadas[0][2];
	}
	else if((x > 84.0 && x < 224.0) && (y >82.0 &&y < 198) && identificaJogadas[1][0] == 0){  //Q4
		identificaJogadas[1][0] = defineJogadas();
		vezJogador = identificaJogadas[1][0];
	}
	else if((x > 232.0 && x < 395.0) && (y >82 &&y < 198) && identificaJogadas[1][1] == 0){  //Q5
		identificaJogadas[1][1] = defineJogadas();
		vezJogador = identificaJogadas[1][1];
	}
	else if((x > 403.0 && x < 588.0) && (y >82 &&y < 198) && identificaJogadas[1][2] == 0){  //Q6
		identificaJogadas[1][2] = defineJogadas();	
		vezJogador = identificaJogadas[1][2];
	}
	else if((x > 84.0 && x < 224.0) && (y >-18 &&y < 78) && identificaJogadas[2][0] == 0){  //Q7
		identificaJogadas[2][0] = defineJogadas();
		vezJogador = identificaJogadas[2][0];
	}
	else if((x > 232.0 && x < 395.0) && (y >-18 &&y < 78) && identificaJogadas[2][1] == 0){  //Q8
		identificaJogadas[2][1] = defineJogadas();
		vezJogador = identificaJogadas[2][1];
	}
	else if((x > 403.0 && x < 588.0) && (y >-18 &&y < 78) && identificaJogadas[2][2] == 0){   //Q9
		identificaJogadas[2][2] = defineJogadas();
		vezJogador = identificaJogadas[2][2];
	} 
	percorreMatriz();
}
void percorreMatriz(){		//função para percorrer a matriz e desenhar as formas alinhadas ao tabuleiro
	
	desenhaFormas(114, 257, identificaJogadas[0][0]);
	
	desenhaFormas(278, 257, identificaJogadas[0][1]);

	desenhaFormas(450, 257, identificaJogadas[0][2]);
	 
	desenhaFormas(114, 140, identificaJogadas[1][0]);
	
	desenhaFormas(278, 140, identificaJogadas[1][1]);
	
	desenhaFormas(450, 140, identificaJogadas[1][2]);
	
	desenhaFormas(114, 25, identificaJogadas[2][0]);
	
	desenhaFormas(278, 25, identificaJogadas[2][1]);
	
	desenhaFormas(450, 25, identificaJogadas[2][2]);
	
}
 
void defineVezJogador(){	//função para definir a vez do jogador, e mostrar o texto no display, tambem mostra o texto se houve vencedor ou empate
	if(fimJogo || !verificaTabuleiro()){
		if(vezJogador %2 == 0){
			glPushMatrix();		
		    	glColor3f(0,0,1);
		    	glTranslatef(180,380,0);
		    	glScalef(0.2, 0.2, 0.2); // diminui o tamanho do fonte
		    	glLineWidth(3); // define a espessura da linha
		    	
		    	if(fimJogo == true){
		    		glColor3f(1,0,0);
						desenhaTexto(GLUT_STROKE_ROMAN,"JOGADOR O VENCEU");
				}
				else{
					desenhaTexto(GLUT_STROKE_ROMAN,"VEZ DO JOGADOR X");		
				}
			glPopMatrix();
			glEnd();
			glFlush();	
		}
		else{
			glPushMatrix();		
		    	glColor3f(1,0,0);
		    	glTranslatef(180,380,0);
		    	glScalef(0.2, 0.2, 0.2); 
		    	glLineWidth(3); 
		    	if(fimJogo == true){
		    		glColor3f(0,0,1);
						desenhaTexto(GLUT_STROKE_ROMAN,"JOGADOR X VENCEU");	
				}
				else{
					desenhaTexto(GLUT_STROKE_ROMAN,"VEZ DO JOGADOR O");		
				}		
			glPopMatrix();
			glEnd();
			glFlush();	
		}
	}	
	else{
		glPushMatrix();		
	    	glColor3f(1,1,1);
	    	glTranslatef(260,380,0);
	    	glScalef(0.2, 0.2, 0.2); 
	    	glLineWidth(3);    	
	    	desenhaTexto(GLUT_STROKE_ROMAN,"EMPATE");	    	
		glPopMatrix();
		glEnd();
		glFlush();
	}
}

int defineJogadas(){        //definir a vez do jogador, 1 para X, 2 para O e 0 nao passou;
	if(vezJogador %2==0){
		return 1; //X
	}
	else {
		return 2;   //O
	}
}

void defineVencedor(){		//verifica se houve três formas iguais na diagonal, horizontal ou vertical e assim risca uma linha aonde houve;
	if((identificaJogadas[0][0] == 1 && identificaJogadas[0][1] == 1 && identificaJogadas[0][2] == 1) ||
	   (identificaJogadas[0][0] == 2 && identificaJogadas[0][1] == 2 && identificaJogadas[0][2] == 2)){
		linhaVencedorHorizontal(295);
	}
	else if((identificaJogadas[1][0] == 1 && identificaJogadas[1][1] == 1 && identificaJogadas[1][2] == 1) ||
			(identificaJogadas[1][0] == 2 && identificaJogadas[1][1] == 2 && identificaJogadas[1][2] == 2)){
		linhaVencedorHorizontal(180);
	}
	else if((identificaJogadas[2][0] == 1 && identificaJogadas[2][1] == 1 && identificaJogadas[2][2] == 1) ||
			(identificaJogadas[2][0] == 2 && identificaJogadas[2][1] == 2 && identificaJogadas[2][2] == 2)){
		linhaVencedorHorizontal(63);
	}
	else if((identificaJogadas[0][0] == 1 && identificaJogadas[1][0] == 1 && identificaJogadas[2][0] == 1) ||
			(identificaJogadas[0][0] == 2 && identificaJogadas[1][0] == 2 && identificaJogadas[2][0] == 2)){
		linhaVencedorVertical(152);
	}
	else if((identificaJogadas[0][1] == 1 && identificaJogadas[1][1] == 1 && identificaJogadas[2][1] == 1) ||
			(identificaJogadas[0][1] == 2 && identificaJogadas[1][1] == 2 && identificaJogadas[2][1] == 2)){
		linhaVencedorVertical(315);
	}
	else if((identificaJogadas[0][2] == 1 && identificaJogadas[1][2] == 1 && identificaJogadas[2][2] == 1) ||
			(identificaJogadas[0][2] == 2 && identificaJogadas[1][2] == 2 && identificaJogadas[2][2] == 2)){
		linhaVencedorVertical(485);
	}	
	else if((identificaJogadas[0][0] == 1 && identificaJogadas[1][1] == 1 && identificaJogadas[2][2] == 1) ||
			(identificaJogadas[0][0] == 2 && identificaJogadas[1][1] == 2 && identificaJogadas[2][2] == 2)){
		linhaVencedorDiagonal(342, 18);
	}
	else if((identificaJogadas[0][2] == 1 && identificaJogadas[1][1] == 1 && identificaJogadas[2][0] == 1) ||
			(identificaJogadas[0][2] == 2 && identificaJogadas[1][1] == 2 && identificaJogadas[2][0] == 2)){
		linhaVencedorDiagonal(18, 342);
	}
}
void linhaVencedorHorizontal(int y){
	glPushMatrix();
		glColor3f(0,1,0);
		glLineWidth(8);
		glBegin(GL_LINES);
			glVertex2f(80, y);
			glVertex2f(560, y);		
		glEnd();
		glFlush();				
	glPopMatrix();
	
	fimJogo = true;
}
void linhaVencedorVertical(int x){
	glPushMatrix();
	glColor3f(0,1,0);
	glLineWidth(8);
	glBegin(GL_LINES);
		glVertex2f(x,350);
		glVertex2f(x,20);	
	glEnd();
	glFlush();				
	glPopMatrix();
	
	fimJogo = true;
}
void linhaVencedorDiagonal(float y, float y1){
	glPushMatrix();
	glColor3f(0,1,0);
	glLineWidth(8);
	glBegin(GL_LINES);
		glVertex2f(80,y);
		glVertex2f(550,y1);	
	glEnd();
	glFlush();				
	glPopMatrix();
	
	fimJogo = true;
}

bool verificaTabuleiro(){		//verifica se todos os quadrados do tabuleiro já foram preenchidos
	int i, j;
	bool completo = true;

	for(i = 0; i < 3; i++){
		for(j = 0; j < 3; j++){
			completo = completo && identificaJogadas[i][j] !=0;
		}
	}
	return completo;
}

void reiniciaJogo(){	//função para reiniciar o jogo;
	int i, j;

	for(i = 0; i < 3; i++){
		for(j = 0; j < 3; j++){
			identificaJogadas[i][j] = 0;
		}
	}
	vezJogador = 0;
	fimJogo = false;
	
	desenhaTabuleiro();
}

