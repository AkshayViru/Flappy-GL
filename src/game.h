#ifndef GAME_H
#define GAME_H

#include <iostream>
#include <vector>

#include <textRenderer.h>
#include <shader.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

enum GameStates { MENU, PLAYING, GAME_OVER };

class Game {
	float GAME_SPEED;
	unsigned int birdTexture, bird_koTexture, bird_45Texture, bird_45DownTexture, bird_DownTexture, bgTexture, bg_koTexture,
					menuBgTexture, pipeTexture, birdVAO, bgVAO, pipeVAO, score, currentPipe;
	glm::vec3 bgCurPos;
	std::vector<glm::vec3> pipeCurPos;
	float deltaTime;
	TextRenderer menuFont, novaFont;

	void play(){
		generateBG();
		generateBird();
		generatePipes();
	}

	void generateBG(bool move = true) {
		if (bgCurPos.x <= -4.0f) {
			bgCurPos.x = 0.0f;
		}
		bgShader.use();

		if(move)
			bgCurPos.x = bgCurPos.x - (GAME_SPEED * deltaTime);
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, bgCurPos);
		bgShader.setMat4("model", model);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, bgTexture);
		glBindVertexArray(bgVAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glm::vec3 bgScrollPos = bgCurPos;
		bgScrollPos.x = bgCurPos.x + 4.0f;
		model = glm::translate(glm::mat4(1.0f), bgScrollPos);
		bgShader.setMat4("model", model);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}

	void generateBird() {
		glm::mat4 model = glm::mat4(1.0f);
		if (flyUpCount == 0) {
			birdCurPos.y = glm::max((float)(birdCurPos.y - deltaTime), -0.77f);
			birdShader.use();

			model = glm::translate(model, birdCurPos);
			birdShader.setMat4("model", model);

			glActiveTexture(GL_TEXTURE0);
			if (birdCurPos.y < fallPoint)
				glBindTexture(GL_TEXTURE_2D, bird_45DownTexture);
			else
				glBindTexture(GL_TEXTURE_2D, birdTexture);
			glBindVertexArray(birdVAO);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		}
		else {
			birdCurPos.y = glm::min((float)(birdCurPos.y + (5*deltaTime)), 0.9f);
			flyUpCount--;
			if (birdCurPos.y == 0.9f)
				flyUpCount = 0;
			birdShader.use();

			model = glm::translate(model, birdCurPos);
			birdShader.setMat4("model", model);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, birdTexture);
			glBindVertexArray(birdVAO);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		}
	}

	void generatePipes(bool move = true) {
		for (auto& curPos : pipeCurPos) {
			if (curPos.x <= -2.5f)
				curPos.x = 1.5f;

			pipeShader.use();

			if (curPos.x >= 1.5f and curPos.x <= 1.55f) {
				curPos.y = (rand() % 50 + 50) / 100.0f;
			}
			if(move)
				curPos.x = curPos.x -(GAME_SPEED * deltaTime);

			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(curPos.x, -curPos.y, 0.0f));
			pipeShader.setMat4("model", model);

			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, pipeTexture);
			glBindVertexArray(pipeVAO);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

			model = glm::mat4(1.0f);
			model = glm::scale(model, glm::vec3(1.0f, -1.0f, 1.0f));
			model = glm::translate(model, glm::vec3(curPos.x, -1.5f + curPos.y, 0.0f));
			pipeShader.setMat4("model", model);

			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		}
	}

	bool checkCollision() {
		float by = birdCurPos.y;
		for (int i = 0; i < pipeCurPos.size(); i++) {
			float px = pipeCurPos[i].x;
			float py = pipeCurPos[i].y;
			if (currentPipe % pipeCurPos.size() == i and px < -0.1f and px > -1.0f) {
				score++;
				currentPipe++;
			}
			if (abs(px - 0.0f) > 0.1f)
				continue;
			if (by + py < 0.6f || by + py > 0.9f) {
				return true;
			}
		}
		return false;
	}

	void gameOver() {
		glm::mat4 model = glm::mat4(1.0f);
		birdCurPos.y = glm::max((float)(birdCurPos.y - (1.5*deltaTime)), -0.77f);

		if (birdCurPos.y <= -0.77f) {
			bgShader.use();

			bgShader.setMat4("model", model);

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, bg_koTexture);
			glBindVertexArray(bgVAO);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

			birdShader.use();

			birdCurPos.y = glm::max((float)(birdCurPos.y - 0.002), -0.77f);

			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(0.0f, -0.77f, 1.0f));;
			birdShader.setMat4("model", model);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, bird_koTexture);
			glBindVertexArray(birdVAO);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

			menuFont.RenderText("GAME OVER", 875.0f, 825.0f, 1.0f, glm::vec3(1.0f));
			menuFont.RenderText("OK", 925.0f, 525.0f, 1.5f, glm::vec3(0.0f));
		}
		else {
			generateBG(false);
			generatePipes(false);

			birdShader.use();
			model = glm::translate(model, birdCurPos);
			birdShader.setMat4("model", model);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, bird_DownTexture);
			glBindVertexArray(birdVAO);
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		}
	}

	void showMenu() {
		bgShader.use();

		glm::mat4 model = glm::mat4(1.0f);
		bgShader.setMat4("model", model);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, menuBgTexture);
		glBindVertexArray(bgVAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		
		if (curOption == 1) {
			menuFont.RenderText("START", 225.0f, 100.0f, 1.5f, glm::vec3(0.0f));
			menuFont.RenderText("HELP", 825.0f, 100.0f, 1.0f, glm::vec3(0.1f));
			menuFont.RenderText("EXIT", 1425.0f, 100.0f, 1.0f, glm::vec3(0.1f));
		}
		else if (curOption == 2) {
			menuFont.RenderText("START", 225.0f, 100.0f, 1.0f, glm::vec3(0.1f));
			menuFont.RenderText("HELP", 825.0f, 100.0f, 1.5f, glm::vec3(0.0f));
			menuFont.RenderText("EXIT", 1425.0f, 100.0f, 1.0f, glm::vec3(0.1f));
		}
		else if (curOption == 3) {
			menuFont.RenderText("START", 225.0f, 100.0f, 1.0f, glm::vec3(0.1f));
			menuFont.RenderText("HELP", 825.0f, 100.0f, 1.0f, glm::vec3(0.1f));
			menuFont.RenderText("EXIT", 1425.0f, 100.0f, 1.5f, glm::vec3(0.0f));
		}
	}

	void showHelp() {
		bgShader.use();

		glm::mat4 model = glm::mat4(1.0f);
		bgShader.setMat4("model", model);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, menuBgTexture);
		glBindVertexArray(bgVAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		novaFont.RenderText("Just Tap to Fly!", 825.0f, 125.0f, 1.0f, glm::vec3(0.0f));
		menuFont.RenderText("Take me Back", 825.0f, 75.0f, 1.0f, glm::vec3(0.0f));
	}

	public:
		Shader bgShader, birdShader, pipeShader;
		unsigned int flyUpCount, curOption;
		float fallPoint;
		GameStates curGameState;
		glm::vec3 birdCurPos;
		bool enterPressed;

		Game(unsigned int birdShaderID, unsigned int bgShaderID, unsigned int pipeShaderID,
			unsigned int birdTexture, unsigned int bird_koTexture, unsigned int bird_45DownTexture, unsigned int bird_DownTexture,
			unsigned int bgTexture, unsigned int bg_koTexture, unsigned int menuBgTexture, unsigned int pipeTexture,
			unsigned int birdVAO, unsigned int bgVAO, unsigned int pipeVAO) 
			: GAME_SPEED(0.4){
			
			this->bgShader.SetID(bgShaderID);
			this->birdShader.SetID(birdShaderID);
			this->pipeShader.SetID(pipeShaderID);
			this->birdTexture = birdTexture;
			this->bird_koTexture = bird_koTexture;
			this->bird_45DownTexture = bird_45DownTexture;
			this->bird_DownTexture = bird_DownTexture;
			this->bgTexture = bgTexture;
			this->bg_koTexture = bg_koTexture;
			this->menuBgTexture = menuBgTexture;
			this->pipeTexture = pipeTexture;
			this->birdVAO = birdVAO;
			this->bgVAO = bgVAO;
			this->pipeVAO = pipeVAO;
			this->menuFont = *(new TextRenderer("fonts/peligroso.otf", "shaders/text.vs", "shaders/text.fs", 0, 48));
			this->novaFont = *(new TextRenderer("fonts/nova.otf", "shaders/text.vs", "shaders/text.fs", 0, 48));
		}

		void init() {
			birdCurPos = glm::vec3(0.0f);
			bgCurPos = glm::vec3(0.0f);
			pipeCurPos = { glm::vec3(1.5f, 0.0f, 0.0f), glm::vec3(2.0f, 0.0f, 0.0f), glm::vec3(2.5f, 0.0f, 0.0f), glm::vec3(3.0f),
											glm::vec3(3.5f, 0.0f, 0.0f), glm::vec3(4.0f, 0.0f, 0.0f), glm::vec3(4.5f, 0.0f, 0.0f), glm::vec3(5.0f, 0.0f, 0.0f) };
			flyUpCount = 0;
			score = 0;
			currentPipe = 0;
			fallPoint = 0.0;
			curGameState = MENU;
			curOption = 1;
			enterPressed = false;
		}

		void run(float deltaTime) {
			//TODO - Rendering is not smooth when actual deltaTime is used
			this->deltaTime = 0.002;
			if (curGameState == MENU) {
				if (enterPressed == false) {
					showMenu();
				}
				else {
					if (curOption == 1) {
						curGameState = PLAYING;
					}
					else if (curOption == 2) {
						showHelp();
					}
				}
			}
			else if (curGameState == PLAYING) {
				play();
				if (birdCurPos.y <= -0.77f || checkCollision()) {
					curGameState = GAME_OVER;
				}
			}
			else if (curGameState == GAME_OVER) {
				gameOver();
			}
		}

		int getScore() {
			return this->score;
		}
};

#endif