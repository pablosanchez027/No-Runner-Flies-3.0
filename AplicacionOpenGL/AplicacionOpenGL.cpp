// AplicacionOpenGL.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include "GL\glew.h"
#include "GLFW\glfw3.h"
#include <math.h>
#include <iostream>

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
static void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos);
static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void salto();
void CharMovement(int direction);


//Declaración de ventana
GLFWwindow *window;
//Propiedades de la ventana
GLfloat ancho = 600;
GLfloat alto = 600;

//valores globales
GLdouble tiempoActual = 0;
GLdouble tiempoPasado = 0;
GLdouble deltaTime = 0;
GLfloat SceneSpeed = 0.2;
GLfloat deltaSpace = 0.3;
GLfloat MousePosX;
GLfloat MousePosY;

//fisicas
GLfloat gravedad = -2.4;
GLfloat speedY;
GLfloat jumpForce = 1;
GLfloat moveSpeed = 1;

//Personaje
	//Posicion
GLfloat CharPosX = 0;
GLfloat CharPosY = 0;

	//Valores
GLfloat tamaño = 0.03;
GLfloat rotation = 0;

GLboolean falling = true;

//latigo
GLfloat latScaleY = 0.01;
GLfloat latScaleX = 0;
GLfloat objetivoX = NULL;
GLfloat objetivoY = NULL;
GLfloat latSpeed = 3;
GLboolean alcanzado = true;
GLfloat latAngulo;
GLfloat maxrange = NULL;
GLboolean recogido = true;
GLboolean atrapar = false;
GLfloat chanceSpace = 0.03;

GLfloat prevObjetivoX;
GLfloat prevObjetivoY;

//Plataforma
struct plataforma {
	GLfloat PlatPosX = 0;
	GLfloat PlatPosY = -0.8;

	//Valores
	GLfloat PlatScaleX = 0.5;
	GLfloat PlatScaleY = 0.03;
};

GLint nplataformas = 1;
plataforma plantas[1];

struct mosca {
	//posicion
	GLfloat MoscaPosX = -2;
	GLfloat MoscaPosY = -2;

	//valores
	GLfloat MoscaScaleX = 0.01;
	GLfloat MoscaScaleY = 0.01;
	GLfloat MoscaSpeed = 1;
	GLdouble Erratidad = 1;
	GLdouble regreso = 0;
	GLfloat angulo = 0;
	GLdouble cambioAngulo = 0;
	GLboolean muerta = false;
};

GLint nMoscas = 30;
mosca mosquitas[30];

void actualizar() 
{
	//Tiempo -----NO TOCAR------
	deltaTime = tiempoActual - tiempoPasado;

	//movimiento personaje
	speedY += gravedad*deltaTime;
	if (falling == true) {
		GLfloat newCharPosY = CharPosY + speedY * deltaTime;
		for (int i = 0; i < nplataformas; i++) {
			if (newCharPosY - tamaño > plantas[i].PlatPosY + plantas[i].PlatScaleY 
				|| CharPosX - tamaño > plantas[i].PlatPosX + plantas[i].PlatScaleX 
				|| CharPosX + tamaño < plantas[i].PlatPosX - plantas[i].PlatScaleX) {
				CharPosY = newCharPosY;
			}
			else {
				newCharPosY = plantas[i].PlatPosY + plantas[i].PlatScaleY + tamaño;
				CharPosY = newCharPosY;
			}
		}
	}
	//coliciones con solidos
	//sobre solido
	for (int i = 0; i < nplataformas; i++) {
		if (CharPosY - tamaño <= plantas[i].PlatPosY + plantas[i].PlatScaleY && CharPosX - tamaño <= plantas[i].PlatPosX + plantas[i].PlatScaleX &&
			CharPosX + tamaño >= plantas[i].PlatPosX - plantas[i].PlatScaleX) {
			falling = false;
			speedY = 0;
			break;
		}
		else {
			falling = true;
		}
	}
	//movimiento
	int right = glfwGetKey(window, GLFW_KEY_A);
	int left = glfwGetKey(window, GLFW_KEY_D);
	if (right == GLFW_PRESS) {
		CharMovement(-1);
	}else if (left == GLFW_PRESS) {
		CharMovement(1);
	}

	//latigo
	if (objetivoX != NULL && objetivoY != NULL) {
		latAngulo = ((atan((objetivoY - CharPosY) / (objetivoX - CharPosX))) * 180 / 3.141592);
		if (latAngulo < 0) latAngulo += 180;
		latScaleX += latSpeed * deltaTime;
		if (latScaleX >= maxrange) {
			alcanzado = true;
			objetivoX = NULL;
			objetivoY = NULL;
		}
	}
	if (alcanzado == true && latScaleX > 0) {
		latScaleX -= latSpeed * deltaTime;
		atrapar = true;
		
	}
	else if (alcanzado == true && latScaleX <= 0) {
		recogido = true;
	}

	//movimiento de la escena
	/*for (int i = 0; i < nplataformas; i++) {
		plantas[i].PlatPosX += -SceneSpeed * deltaTime;
	}*/

	//Genera plataformas
	/*for (int i = 0; i < nplataformas; i++) {
		//std::cout << plantas[i].PlatPosX + plantas[i].PlatScaleX << " , ";
		if (plantas[i].PlatPosX + plantas[i].PlatScaleX < -1) {
			plantas[i].PlatPosY = (float)(rand() % 800)/500 - 1;
			plantas[i].PlatScaleX = (float)(rand() % 1000 + 1) / 500 - 1;
			plantas[i].PlatPosX = 1 + plantas[i].PlatScaleX;
		}
	}*/

	//moscas
	for (int i = 0; i < nMoscas; i++) {
		//GeneraMoscas
		if (mosquitas[i].MoscaPosX == -2 && mosquitas[i].MoscaPosY == -2 && mosquitas[i].regreso <= 0) {
			mosquitas[i].MoscaPosX = (float)(rand() % 1000) / 500 - 1;
			mosquitas[i].MoscaPosY = (float)(rand() % 700) / 500 - 0.7;
			mosquitas[i].Erratidad = (double)(rand() % 10);
			mosquitas[i].MoscaSpeed = (float)(rand() % 20) / 10;
		}
		//movimiento
		if (mosquitas[i].muerta == false) {
			mosquitas[i].cambioAngulo -= deltaTime;
			if (mosquitas[i].cambioAngulo <= 0) mosquitas[i].angulo = (float)(rand() % 36000) / 100;
			mosquitas[i].MoscaPosX += mosquitas[i].MoscaSpeed*cos(mosquitas[i].angulo) * deltaTime;
			mosquitas[i].MoscaPosY += mosquitas[i].MoscaSpeed*sin(mosquitas[i].angulo) * deltaTime;
			
		}
		//matar mosquita
		if (atrapar == true && mosquitas[i].MoscaPosX <= prevObjetivoX + chanceSpace && mosquitas[i].MoscaPosX >= prevObjetivoX - chanceSpace
			&& mosquitas[i].MoscaPosY <= prevObjetivoY + chanceSpace && mosquitas[i].MoscaPosY >= prevObjetivoY - chanceSpace) {
			mosquitas[i].muerta = true;
			mosquitas[i].regreso = 5;
			mosquitas[i].MoscaPosX = -2;
			mosquitas[i].MoscaPosY = -2;
		}
		//mosca regresa
		if (mosquitas[i].muerta == true) {
			mosquitas[i].regreso -= deltaTime;
		}
	}

	atrapar = false;
	//std::cout << std::endl;

	//GAMEOVER
	//if (CharPosY + tamaño < -1) exit(0);

	//Tiempo -----NO TOCAR------
	tiempoPasado = tiempoActual;
}

void dibujar()
{
	//latigo
	glPushMatrix();
		glTranslatef(CharPosX, CharPosY, 0);
		glRotatef(latAngulo, 0, 0, 1);
			glBegin(GL_TRIANGLE_STRIP);
				glColor3f(0, 0, 1);
				glVertex3f(latScaleX, latScaleY, 1);
				glVertex3f(-0, latScaleY, 1);
				glVertex3f(latScaleX, -latScaleY, 1);
				glVertex3f(-0, -latScaleY, 1);
			glEnd();
	glPopMatrix();

	//personaje
	glPushMatrix();
		glTranslatef(CharPosX, CharPosY, 0);
		glRotatef(rotation, 0, 0, 1);
			glBegin(GL_TRIANGLE_STRIP);
				glColor3f(1, 0, 0);
				glVertex3f(tamaño,tamaño,1);
				glVertex3f(-tamaño, tamaño, 1);
				glVertex3f(tamaño, -tamaño, 1);
				glVertex3f(-tamaño, -tamaño, 1);
			glEnd();
	glPopMatrix();

	//moscas
	for (int i = 0; i < nMoscas; i++) {
		glPushMatrix();
			glTranslatef(mosquitas[i].MoscaPosX, mosquitas[i].MoscaPosY, 0);
			glRotatef(mosquitas[i].angulo, 0, 0, 1);
				glBegin(GL_TRIANGLE_STRIP);
					glColor3f(0.2, 0.2, 0.2);
					glVertex3f(-mosquitas[i].MoscaScaleX, 0, 0);
					glVertex3f(0, -mosquitas[i].MoscaScaleY, 0);
					glVertex3f(0, mosquitas[i].MoscaScaleY, 0);
					glVertex3f(mosquitas[i].MoscaScaleX, 0, 0);
				glEnd();
		glPopMatrix();
	}

	//plataforma
	for (int i = 0; i < nplataformas; i++) {
		glPushMatrix();
			glTranslatef(plantas[i].PlatPosX, plantas[i].PlatPosY, 0);
				glBegin(GL_TRIANGLE_STRIP);
					glColor3f(0, 0, 0);
					glVertex3f(plantas[i].PlatScaleX, plantas[i].PlatScaleY, 0);
					glVertex3f(-plantas[i].PlatScaleX, plantas[i].PlatScaleY, 0);
					glVertex3f(plantas[i].PlatScaleX, -plantas[i].PlatScaleY, 0);
					glVertex3f(-plantas[i].PlatScaleX, -plantas[i].PlatScaleY, 0);
				glEnd();
		glPopMatrix();
	}
}

int main()
{


	//Inicialización de GLFW
	if (!glfwInit())
	{
		//Si no inició bien, terminar el programa
		exit(EXIT_FAILURE);
	}

	//Inicializar la ventana
	window = glfwCreateWindow(ancho, alto, "Mi ventana", NULL, NULL);
	//Verificar la creación de la ventana
	if (!window) {
		//Terminó con GLFW y cierro el programa
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	//Establecer "window" como el contexto de OpenGL
	glfwMakeContextCurrent(window);
	//callback key
	glfwSetKeyCallback(window, GLFWkeyfun(key_callback));
	//callback cursor
	glfwSetCursorPosCallback(window, cursor_pos_callback);
	//callback boton mouse
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	
	//genera plataformas
	/*for (int i = 0; i < nplataformas; i++) {
		float newPlatPosX = (float)(rand() % 1000 + 1) / 500 - 1;
		float newPlatScalePosX = (float)(rand() % 1000 + 1) / 500 - 1;
		for (int ii = 0; ii < nplataformas; ii++) {
			if (!(ii == i)) {
				if ((newPlatPosX - newPlatScalePosX) - (plantas[ii].PlatPosX + plantas[ii].PlatScaleX) < deltaSpace) {
					newPlatPosX = (float)(rand() % 1000 + 1) / 500 - 1;
					newPlatScalePosX = (float)(rand() % 1000 + 1) / 500 - 1;
					ii = 0;
				}
			}
		}
		plantas[i].PlatPosX = newPlatPosX;
		plantas[i].PlatScaleX = newPlatScalePosX;
		plantas[i].PlatPosY = (float)(rand() % 800 + 1) / 500 - 1;
	}*/

	//Draw Loop
	//Continua mientras la ventana se encuentre abierta
	while (!glfwWindowShouldClose(window))
	{
		tiempoActual = glfwGetTime();
		//Establecer el area del render
		glViewport(0, 0, ancho, alto);
		//Establecer el color con el que se limpia la pantalla
		glClearColor(0, 1, 0, 1);
		//Limpiar pantalla
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Rutina de dibujo
		actualizar();
		dibujar();

		//Intercambio de buffers
		glfwSwapBuffers(window);
		//Buscar señales de entrada
		glfwPollEvents();
	}

	return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_W && action == GLFW_PRESS) {
		salto();
	}
}

static void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos) {
	MousePosX = ((xpos / ancho) * 2) - 1;
	MousePosY = (((ypos / alto) * 2) - 1)*(-1);
	//std::cout << MousePosX << " , " << MousePosY << std::endl;
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		std::cout << alcanzado << std::endl;
		if (alcanzado == true && recogido == true) {
			objetivoX = MousePosX;
			objetivoY = MousePosY;
			prevObjetivoX = objetivoX;
			prevObjetivoY = objetivoY;
			alcanzado = false;
			recogido = false;
			maxrange = sqrtf(powf((objetivoX - CharPosX), 2) + powf((objetivoY - CharPosY), 2));
			
		}
		
	}
}

void salto() {
	if (falling == false) {
		speedY = jumpForce;
		falling = true;
	}
}

void CharMovement(int direction) {
	GLfloat newCharPosX = CharPosX + moveSpeed * deltaTime * direction;
	//derecha
	if (direction == 1) {
		for (int i = 0; i < nplataformas; i++) {
			if (!(newCharPosX + tamaño > plantas[i].PlatPosX - plantas[i].PlatScaleX)
				|| !(CharPosY - tamaño < plantas[i].PlatPosY + plantas[i].PlatScaleY)
				|| !(newCharPosX - tamaño < plantas[i].PlatPosX + plantas[i].PlatScaleX)) {
				CharPosX = newCharPosX;
			}
		}
	}
	//izquierda
	else if (direction == -1) {
		for (int i = 0; i < nplataformas; i++) {
			if (!(newCharPosX + tamaño > plantas[i].PlatPosX - plantas[i].PlatScaleX)
				|| !(CharPosY - tamaño < plantas[i].PlatPosY + plantas[i].PlatScaleY)
				|| !(newCharPosX - tamaño < plantas[i].PlatPosX + plantas[i].PlatScaleX)) {
				CharPosX = newCharPosX;
			}
		}
	}
}