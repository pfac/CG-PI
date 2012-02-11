#include "keyboard.h"

#include <ctype.h>

namespace cg
{
	namespace keyboard
	{
		void keyDown(unsigned char key);
		void keyUp(unsigned char key);
		bool isKeyDown(unsigned char key);



		bool lc_state[26];
		Player *player;

		void init()
		{
			for ( int k = 0 ; k < 26 ; ++k )
			{
				lc_state[k] = false;
			}
			player = NULL;
		}

		void init(Player *p)
		{
			init();
			player = p;
		}

		void keyDown(unsigned char key, int xx, int yy)
		{
			if ( ! isKeyDown(key) )
			{
				switch (key)
				{
				case 'w':
					if ( isKeyDown('s') )
						player->stop();
					else
						player->forward();
					break;
				case 's':
					if ( isKeyDown('w') )
						player->stop();
					else
						player->backward();
					break;

				case 'a':
					if ( isKeyDown('d') )
						player->straight();
					else
						player->left();
					break;
				case 'd':
					if ( isKeyDown('a') )
						player->straight();
					else
						player->right();
				}
				keyDown(key);
			}
		}

		void keyDown(unsigned char key)
		{
			if ( islower(key) )
				lc_state[ key - 'a' ] = true;
		}

		void keyUp(unsigned char key, int xx, int yy)
		{
			switch (key)
			{
			case 'w':
				if ( isKeyDown('s') )
					player->backward();
				else
					player->stop();
				break;
			case 's':
				if ( isKeyDown('w') )
					player->forward();
				else
					player->stop();
				break;

			case 'a':
				if ( isKeyDown('d') )
					player->right();
				else
					player->straight();
				break;
			case 'd':
				if ( isKeyDown('a') )
					player->left();
				else
					player->straight();
			}
			keyUp(key);
		}

		void keyUp(unsigned char key)
		{
			if ( islower(key) )
				lc_state[ key - 'a' ] = false;
		}

		bool isKeyDown(unsigned char key)
		{
			if ( islower(key) )
				return lc_state[ key - 'a' ];
			return false;
		}
	}
}