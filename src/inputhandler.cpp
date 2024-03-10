// #include "game.h"

// void KeyboardHandler::PlayerInputHandler(Player & player, Keys & keys){
//     if (Game::event.type==SDL_KEYDOWN){
//         auto key = Game::event.key.keysym.sym;
//         if (key == keys.right){
//             player.key_right = true;
//         }
//         else if (key == keys.left){
//             player.key_left = true;
//         }
//         else if (key == keys.down){
//             player.key_down = true;
//         }
//         else if (key == keys.up){
//             player.key_up = true;
//         }
//         else if (key == keys.space){
//             if (player.collide_down){
//                 player.collide_down = false;
//                 player.velocity.d = -Game::jump_speed;
//                 player.position.y += player.velocity.d;
//                 player.previous_state = player.current_state
//                 ;
//                 player.current_state = JUMP;
//                 playSound("jump", jump_channel, 0);
//             }
//         }
//     }
//     else if (Game::event.type==SDL_KEYUP){
//         auto key = Game::event.key.keysym.sym;
//         if (key == keys.right){
//             player.key_right = false;
//         }
//         else if (key == keys.left){
//             player.key_left = false;
//         }
//         else if (key == keys.down){
//             player.key_down = false;
//         }
//         else if (key == keys.up){
//             player.key_up = false;
//         }
//         else if (key == keys.space){

//         }
//     }
// }