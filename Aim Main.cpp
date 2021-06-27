//You need Vector3 struct
//Offsets are work in 0.15.8 Standoff 2 version

static void *get_aim(void *player) {
    return *(void **)((uint64_t) player + 0x2C);
}

Quaternion PlayerLook;
void* myPlayer = NULL;
void *enemyPlayer = NULL;

void (*old_playerupdate)(void* player);
void playerupdate(void* player) {
    if (player != NULL) {
        if (IsLocal(player)) {
            myPlayer = player;
        }

        if (myPlayer) {
            if (GetPlayerTeam(myPlayer) != GetPlayerTeam(player)) {
                enemyPlayer = player;
            }
            if (aim && enemyPlayer && !isPlayerDead(enemyPlayer)) {
                void* aimingdata = *(void **)((uint64_t) get_aim(myPlayer) + 0x5C);
                if (aimingdata) {
                    Vector3 angle = ToEulerRad(PlayerLook);
                    if (angle.x >= 275.0f)
                        angle.x -= 360.0f;
                    if (angle.x <= -275.0f)
                        angle.x += 360.0f;
                    *(Vector3 *)((uint64_t)aimingdata + 0x10) = angle;
                    *(Vector3 *)((uint64_t)aimingdata + 0x1C) = angle;
                }
            }
        }
        if(enemyPlayer) {
            if(!isPlayerDead(enemyPlayer)) {
                if (aim) {
                    PlayerLook = Quaternion::LookRotation((GetPlayerLocation(enemyPlayer) + Vector3(0, -0.40f, 0)) - GetPlayerLocation(myPlayer), Vector3(0, 1, 0));
                }
            }
        } else {
            enemyPlayer = NULL;
            return;
      }
    }
old_playerupdate(player);
}



MSHookFunction((void *) getAbsoluteAddress(targetLibName, 0x98F4F78), (void *) playerupdate, (void **) &old_playerupdate);
IsLocal = (bool (*)(void *)) getAbsoluteAddress(targetLibName, 0x18F100C);
GetPlayerHealth = (int (*)(void *)) getAbsoluteAddress(targetLibName, 0x18F184C);
GetPlayerTeam = (int (*)(void *)) getAbsoluteAddress(targetLibName, 0x18F3374);