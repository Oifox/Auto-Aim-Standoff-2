float NormalizeAngle (float angle){
    while (angle>360)
        angle -= 360;
    while (angle<0)
        angle += 360;
    return angle;
}

Vector3 NormalizeAngles (Vector3 angles){
    angles.x = NormalizeAngle (angles.x);
    angles.y = NormalizeAngle (angles.y);
    angles.z = NormalizeAngle (angles.z);
    return angles;
}

Vector3 ToEulerRad(Quaternion q1){
    float Rad2Deg = 360.0f / (M_PI * 2.0f);

    float sqw = q1.w * q1.w;
    float sqx = q1.x * q1.x;
    float sqy = q1.y * q1.y;
    float sqz = q1.z * q1.z;
    float unit = sqx + sqy + sqz + sqw;
    float test = q1.x * q1.w - q1.y * q1.z;
    Vector3 v;

    if (test>0.4995f*unit) {
        v.y = 2.0f * atan2f (q1.y, q1.x);
        v.x = M_PI / 2.0f;
        v.z = 0;
        return NormalizeAngles(v * Rad2Deg);
    }
    if (test<-0.4995f*unit) {
        v.y = -2.0f * atan2f (q1.y, q1.x);
        v.x = -M_PI / 2.0f;
        v.z = 0;
        return NormalizeAngles (v * Rad2Deg);
    }
    Quaternion q(q1.w, q1.z, q1.x, q1.y);
    v.y = atan2f (2.0f * q.x * q.w + 2.0f * q.y * q.z, 1 - 2.0f * (q.z * q.z + q.w * q.w)); // yaw
    v.x = asinf (2.0f * (q.x * q.z - q.w * q.y)); // pitch
    v.z = atan2f (2.0f * q.x * q.y + 2.0f * q.z * q.w, 1 - 2.0f * (q.y * q.y + q.z * q.z)); // roll
    return NormalizeAngles (v * Rad2Deg);
}

int (*GetPlayerTeam)(void* player);
int (*GetPlayerHealth)(void* player);
bool (*IsLocal)(void* player);

//You need Vector3 struct
//Offsets are work in 0.15.10 Standoff 2 version

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
