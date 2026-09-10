/*!****************************************************************************
 * @file map_template.cppm
 * @brief Contains all the in-game text in one place, so it's easy to follow 
 *        and translate.
 *******************************************************************************/        

module;
export module game.text;

import engine;
import game.data;
import game.options;
import game.dialogue_box;

using namespace gba;

#include "options.hpp"

namespace game {

// ------------------------------ Main Menu -------------------------------- //

namespace main_menu {

export constexpr u32 OP_COUNT = 4;
export const c32* const MENU_TEXT[LANG_COUNT][OP_COUNT] = {
    { U"^1New Game",
      U"^2Skip Intro",
      U"^3Continue", 
      U"^4Language" },
    { U"^1Нова Гра",
      U"^2Без Заставки",
      U"^3Продовжити", 
      U"^4Мова" },
    { U"^1新しいゲーム",
      U"^2イントロをスキップ",
      U"^3ゲームのつづき",
      U"^4言語"},
};

} // namespace main_menu


// -------------------------------- Intro ---------------------------------- //

namespace intro {

/// @brief Intro text block
export struct TextBlock {
    const c32* const text;
    const bool clear;
    const fix32 speed;
    const u32 other;
};

// Part 1 Text

export const TextBlock PART_1_ENG[] =
        { { U"\n\n^1Somewhere   \nin the far future.\n\n", true, MEDIUM, 0 }
        , { U"Humanity broke the\nchains of the planet Earth\nand conquered the space.", 
            false, MEDIUM, 0 }
        , { 0, true, FIX32_ZERO, 0 } };

export const TextBlock PART_1_UKR[] =
        { { U"\n^1Десь   \nу далекому\nмайбутньому.\n\n", true, MEDIUM, 0 }
        , { U"Людство вирвалось\nз оков планети Земля\nта опанувало космос.", 
            false, MEDIUM, 0 }
        , { 0, true, FIX32_ZERO, 0 } };

export const TextBlock PART_1_JAP[] =
        { { U"\n^1ここは   \n遠い未来です。\n^#", true, MEDIUM, 0 }
        , { U"地球の鎖を外れたあとで\n人類は宇宙空間を\nマスターすれた。", 
            false, MEDIUM, 0 }
        , { 0, true, FIX32_ZERO, 0 } };

export const TextBlock* PART_1_TEXTBLOCKS[LANG_COUNT] =
        { PART_1_ENG, PART_1_UKR, PART_1_JAP };

// Part 2 Text

export const TextBlock PART_2_ENG[] = 
        { { U"^1My name is ^3" MAINC_NAME_ENG "^1.\n^#", true, MEDIUM, 0 }
        , { U"I'm a third\ngeneration\nspace duchess.\n^#", false, MEDIUM, 0 }
        , { U"Meaning\nthat I have\nno money.", false, MEDIUM, 0 }
        , { 0, true, FIX32_ZERO, 0 } };

export const TextBlock PART_2_UKR[] = 
        { { U"^1Мене звати ^3" MAINC_NAME_UKR "^1.\n^#", true, MEDIUM, 0 }
        , { U"Я космічна\nгерцогиня у\nтретьому\nпоколінні.\n^#", false, MEDIUM, 0 }
        , { U"Що насправді\nозначає, що у мене\nнемає грошей.", false, MEDIUM, 0 }
        , { 0, true, FIX32_ZERO, 0 } };

export const TextBlock PART_2_JAP[] = 
        { { U"^1私の名前は^3" MAINC_NAME_JAP "^1。\n^#", true, MEDIUM, 0 }
        , { U"三世の宇宙空間\nの貴族女性\nあたしは。\n^#", false, MEDIUM, 0 }
        , { U"お金がない\nという意味だ.", false, MEDIUM, 0 }
        , { 0, true, FIX32_ZERO, 0 } };

export const TextBlock* PART_2_TEXTBLOCKS[LANG_COUNT] = 
        { PART_2_ENG, PART_2_UKR, PART_2_JAP }; 

// Part 3 Text

export const TextBlock PART_3_ENG[] = 
        { { U"^1That's why I'm\nworking in the delivery.\n^#", true, MEDIUM, 0 }
        , { U"The work is boring,\nbecause everything\nis automated.\n^#", false, MEDIUM, 0 }
        , { U"I'm here just in case.\n", false, MEDIUM, 0 }
        , { U"Well, \"in case\" ^4is now^1!", false, MEDIUM, 0 }
        , { U"    ", true, SLOWEST, 0 }
        , { 0, true, FIX32_ZERO, 0 } };

export const TextBlock PART_3_UKR[] = 
        { { U"^1Тому приходиться\nпідробляти кур'єром.\n^#", true, MEDIUM, 0 }
        , { U"Робота вкрай нудна,\nадже все повністю\nавтоматизоване.\n^#", false, MEDIUM, 0 }
        , { U"Я тут на всяк випадок.\n", false, MEDIUM, 0 }
        , { U"І цей випадок ^4настав^1!", false, MEDIUM, 0 }
        , { U"    ", true, SLOWEST, 0 }
        , { 0, true, FIX32_ZERO, 0 } };

export const TextBlock PART_3_JAP[] = 
        { { U"^1だから宅急便のバ^-^-イ^-^-ト^-^-をす^-^-る.\n^#", true, MEDIUM, 0 }
        , { U"でも、自動のせいで\n^-^-と^-^-て^-^-もつまらない仕事だよ^-^-。\n^#", 
            false, MEDIUM, 0 }
        , { U"念のために私がここでい^-^-る^-^-。\n", false, MEDIUM, 0 }
        , { U"じゃあ、「念のために」は\n^4ここだ^1！", false, MEDIUM, 0 }
        , { U"    ", true, SLOWEST, 0 }
        , { 0, true, FIX32_ZERO, 0 } };

export const TextBlock* PART_3_TEXTBLOCKS[LANG_COUNT] = 
        { PART_3_ENG, PART_3_UKR, PART_3_JAP }; 

// Part 4 Text

export const TextBlock PART_4_ENG[] = 
        { { U"^1An explosion inside\nthe belly of the ship\n"
            U"scattered the cargo across\nthe nearby planets.\n^#", 
            true, MEDIUM, 0 }
        , { U"My mission is to\nfind them and get them\n"
            U"back onto the ship.\n^#", false, MEDIUM, 0 }
        , { 0, true, FIX32_ZERO, 0 } };

export const TextBlock PART_4_UKR[] = 
        { { U"^1Вибух в вантажному\nвідділенні виштовхнув\n"
            U"посилки у відкритий\nкосмос, і вони попадали\n"
            U"на найближчі планети.\n^#", true, MEDIUM, 0 }
        , { U"Моє завдання - знайти\nпосилки та повернути їх\n"
            U"на космічний корабель.\n^#", false, MEDIUM, 0 }
        , { 0, true, FIX32_ZERO, 0 } };

export const TextBlock PART_4_JAP[] = 
        { { U"^1爆発のせいで\n近くの惑星たちに\n貨物を散りました。\n^#", 
            true, MEDIUM, 0 }
        , { U"そして、あたしは\nす^-べ^-て貨物が探して\n"
            U"戻らなければならない。\n^#", false, MEDIUM, 0 }
        , { 0, true, FIX32_ZERO, 0 } };

export const TextBlock* PART_4_TEXTBLOCKS[LANG_COUNT] = 
        { PART_4_ENG, PART_4_UKR, PART_4_JAP }; 

} // namespace intro


namespace map {

// ------------------------------ Factory 00 ------------------------------- //

namespace factory_00 {

// Beginning Dialogue

export const dlg_box::TextBlock TB_BEGINNING_ENG[] = 
    { { dlg_box::Actor::PILOT, 
        U"\"Lady " MAINC_NAME_ENG U"! It's ^4" PILOT_NAME_ENG 
        U"^1 - the first\npilot of your spaceship", 
        MEDIUM }
    , { dlg_box::Actor::PILOT, 
        U"...", 
        SLOWEST }
    , { dlg_box::Actor::PILOT, 
        U"...and also your biggest fan!\"", 
        MEDIUM }
    , { dlg_box::Actor::MAINC,
        U"\"E-e-e...\"", 
        MEDIUM }
    , { dlg_box::Actor::PILOT, 
        U"\"Going by the protocol. Reminder: \narrows - moving left/right", 
        MEDIUM }
    , { dlg_box::Actor::PILOT,
        U"button ^4A^1 when running\n- a long jump.", 
        MEDIUM }
    , { dlg_box::Actor::PILOT,
        U"Warning - the movement is\ninertial.\"", 
        MEDIUM }
    , { dlg_box::Actor::MAINC,
        U"\"Roger...\"",
        MEDIUM }
    , { dlg_box::Actor::PILOT,
        U"\"I'll be on the line all the time!\"",
        MEDIUM }
    , { dlg_box::Actor::MAINC,
        U"\"Gotcha.\"",
        MEDIUM }
    , { dlg_box::Actor::NONE, 0, FIX32_ZERO } 
    };

export const dlg_box::TextBlock TB_BEGINNING_UKR[] = 
    { { dlg_box::Actor::PILOT, 
        U"\"Пані " MAINC_NAME_UKR U"! На зв'язку ^4" PILOT_NAME_UKR 
        U"^1 - \nперший пілот вашого корабля", 
        MEDIUM }
    , { dlg_box::Actor::PILOT, 
        U"...", 
        SLOWEST }
    , { dlg_box::Actor::PILOT, 
        U"...та ваш найбільший\nшанувальник!\"", 
        MEDIUM }
    , { dlg_box::Actor::MAINC,
        U"\"Е-е-е...\"", 
        MEDIUM }
    , { dlg_box::Actor::PILOT, 
        U"\"Виконую протокол. Нагадую: \nстрілки - рух вліво/вправо", 
        MEDIUM }
    , { dlg_box::Actor::PILOT,
        U"кнопка ^4А^1 під час бігу - стрибок.\nОбережно, рух має інерцію.\"", 
        MEDIUM }
    , { dlg_box::Actor::MAINC,
        U"\"Зрозуміло...\"", 
        MEDIUM }
    , { dlg_box::Actor::PILOT,
        U"\"Буду весь час на зв'язку!\"",
        MEDIUM }
    , { dlg_box::Actor::MAINC,
        U"\"Гаразд.\"", 
        MEDIUM }
    , { dlg_box::Actor::NONE, 0, FIX32_ZERO } 
    };

export const dlg_box::TextBlock TB_BEGINNING_JAP[] = 
    { { dlg_box::Actor::PILOT, 
        U"「" MAINC_NAME_JAP U"様、初めまして。",
        MEDIUM }
    , { dlg_box::Actor::PILOT, 
        U"^4" PILOT_NAME_JAP U"^1と申します。機長と",
        MEDIUM }
    , { dlg_box::Actor::PILOT, 
        U"...", 
        SLOWEST }
    , { dlg_box::Actor::PILOT, 
        U"...最大のファンです！」", 
        MEDIUM }
    , { dlg_box::Actor::MAINC,
        U"「ええと」", 
        MEDIUM }
    , { dlg_box::Actor::PILOT, 
        U"「プロトコルをします。", 
        MEDIUM }
    , { dlg_box::Actor::PILOT, 
        U"ルールが簡単。", 
        MEDIUM }
    , { dlg_box::Actor::PILOT, 
        U"十字キーは左と右に走ります。", 
        MEDIUM }
    , { dlg_box::Actor::PILOT,
        U"そ^-の時に^4A^1ボタンはジ^-^-ャ^-ン^-プす^-る。^-^-^-", 
        MEDIUM }
    , { dlg_box::Actor::PILOT,
        U"でも、気を付けて", 
        MEDIUM }
    , { dlg_box::Actor::PILOT,
        U"動くことは慣性があります。」", // kansei ga arimasu (has inertia)
        MEDIUM }
    , { dlg_box::Actor::MAINC,
        U"「分かりました。」",
        MEDIUM }
    , { dlg_box::Actor::PILOT,
        U"^-^-^-「^-^-^-^-じ^-^-ゃ^-あ、^-^-僕はい^-つ^-も電話でい^-ま^-す^-^-^-^-」^-^-^-^-^-",
        MEDIUM }
    , { dlg_box::Actor::MAINC,
        U"「はい！」", 
        MEDIUM }
    , { dlg_box::Actor::NONE, 0, FIX32_ZERO } 
    };

export const dlg_box::TextBlock* const TB_BEGINNING[LANG_COUNT] = {
    TB_BEGINNING_ENG,
    TB_BEGINNING_UKR,
    TB_BEGINNING_JAP
};


// Dialogue after the first respawn

export const dlg_box::TextBlock TB_RESPAWN_ENG[] = 
    { { dlg_box::Actor::PILOT, 
        U"\"Lady " MAINC_NAME_ENG U". This is your first\n^4respawn^1.", 
        MEDIUM }
    , { dlg_box::Actor::PILOT,
        U"Every time you fall out the map,\nor in any other dangers",
        MEDIUM }
    , { dlg_box::Actor::PILOT,
        U"it activates automatically and\nteleports you back to safety.\"",
        MEDIUM }
    , { dlg_box::Actor::MAINC,
        U"\"Roger.\"",
        MEDIUM }
    , { dlg_box::Actor::PILOT,
        U"\"A special sign is shown\nwhen respawn point changes.\"",
        MEDIUM }
    , { dlg_box::Actor::MAINC,
        U"\"Yes, I see a sign. Thanks.\"",
        MEDIUM }
    , { dlg_box::Actor::NONE, 0, FIX32_ZERO }
    };

export const dlg_box::TextBlock TB_RESPAWN_UKR[] = 
    { { dlg_box::Actor::PILOT, 
        U"\"Пані " MAINC_NAME_UKR U". Це ваш перший\n^4респавн^1.", 
        MEDIUM }
    , { dlg_box::Actor::PILOT, 
        U"Кожного разу як ви випадаєте\nза межі карти,", 
        MEDIUM }
    , { dlg_box::Actor::PILOT, 
        U"або при інших небезпеках,\nвін активується автоматично", 
        MEDIUM }
    , { dlg_box::Actor::PILOT, 
        U"та перекидає вас у найближче\nбезпечне місце.\"", 
        MEDIUM }
    , { dlg_box::Actor::MAINC,
        U"\"Зрозуміло.\"",
        MEDIUM }
    , { dlg_box::Actor::PILOT,
        U"\"Спец-значок з'являється\nпри зміні точки респавну.\"", 
        MEDIUM }
    , { dlg_box::Actor::MAINC,
        U"\"Так, був значок. Дякую.\"",
        MEDIUM }
    , { dlg_box::Actor::NONE, 0, FIX32_ZERO } 
    };

export const dlg_box::TextBlock TB_RESPAWN_JAP[] = 
    { { dlg_box::Actor::PILOT, 
        U"「" MAINC_NAME_JAP U"様。",
        MEDIUM }
    , { dlg_box::Actor::PILOT, 
        U"今は初めての^4スポーン^1でした。", 
        MEDIUM }
    , { dlg_box::Actor::PILOT, 
        U"ステージの外で落ちたら、", 
        MEDIUM }
    , { dlg_box::Actor::PILOT, 
        U"危険すぎる時たら、",
        MEDIUM }
    , { dlg_box::Actor::PILOT, 
        U"スポーンが点けもらいます。」",
        MEDIUM }
    , { dlg_box::Actor::MAINC,
        U"「分かりました。」",
        MEDIUM }
    , { dlg_box::Actor::PILOT,
        U"「スポーンの場所を替える時、", 
        MEDIUM }
    , { dlg_box::Actor::PILOT,
        U"ス^-ポ^-ー^-^-ンのア^-^-イ^-^-コ^-^-ンが現れます^-^-。^-^-^-」^-^-", 
        MEDIUM }
    , { dlg_box::Actor::MAINC,
        U"^-^-^-「^-^-はい、^-^-見えます。ありがとう^-^-。^-^-^-^-」^-^-",
        MEDIUM }
    , { dlg_box::Actor::NONE, 0, FIX32_ZERO } 
    };


export const dlg_box::TextBlock* const TB_RESPAWN[LANG_COUNT] = {
    TB_RESPAWN_ENG,
    TB_RESPAWN_UKR,
    TB_RESPAWN_JAP
};


// Shooting and Mines

export const dlg_box::TextBlock TB_MINES_ENG[] =
    { { dlg_box::Actor::PILOT, 
        U"\"Lady " MAINC_NAME_ENG U", be careful! Mines\nahead!", 
        MEDIUM }
    , { dlg_box::Actor::PILOT, 
        U"It is better to detonate them\nfrom the distance.", 
        MEDIUM }
    , { dlg_box::Actor::PILOT,
        U"Hold ^4R^1 to switch into the\nshooting mode.", 
        MEDIUM }
    , { dlg_box::Actor::PILOT,
        U"Use arrows to move the cursor.", 
        MEDIUM }
    , { dlg_box::Actor::PILOT, 
        U"Aim carefully, then press ^4A^1\nto fire the gun.\"", 
        MEDIUM }
    , { dlg_box::Actor::MAINC,
        U"\"The pistol is so-so. You can't\nshoot very far.\"", 
        MEDIUM }
    , { dlg_box::Actor::MAINC,
        U"\"The clip is small, and the\nrecharge is slow.\"", 
        MEDIUM }
    , { dlg_box::Actor::PILOT,
        U"\"By the way, the explosion also\nscattered some weapon parts.", 
        MEDIUM }
    , { dlg_box::Actor::PILOT,
        U"Big chance you can find\nsomething interesing!\"", 
        MEDIUM }
    , { dlg_box::Actor::MAINC,
        U"\"O-o-o, understood. Thanks!\"",
        MEDIUM }
    , { dlg_box::Actor::PILOT,
        U"\"No problems!\"", 
        MEDIUM }
    , { dlg_box::Actor::NONE, 0, FIX32_ZERO } 
    };

export const dlg_box::TextBlock TB_MINES_UKR[] =
    { { dlg_box::Actor::PILOT, 
        U"\"Пані " MAINC_NAME_UKR U", обережно! Попереду\nміни.", 
        MEDIUM }
    , { dlg_box::Actor::PILOT, 
        U"Їх можна безпечно знешкодити\nна відстані.", 
        MEDIUM }
    , { dlg_box::Actor::PILOT,
        U"Тримайте ^4R^1 для переходу у\nрежим стрільби.", 
        MEDIUM }
    , { dlg_box::Actor::PILOT,
        U"Стрілки - рух курсором.", 
        MEDIUM }
    , { dlg_box::Actor::PILOT, 
        U"Добре прицільтесь та\nнатисніть ^4A^1 для пострілу.\"", 
        MEDIUM }
    , { dlg_box::Actor::MAINC,
        U"\"Пістолет такий собі.\nДакело не стрільнеш.\"", 
        MEDIUM }
    , { dlg_box::Actor::MAINC,
        U"\"Обойма невелика, та\nповільно перезаряджається.\"", 
        MEDIUM }
    , { dlg_box::Actor::PILOT,
        U"\"Доречі, вибух порозсипав\nі оновки до зброї.", 
        MEDIUM }
    , { dlg_box::Actor::PILOT,
        U"Тому є великий шанс знайти\nщось цікавіше!\"", 
        MEDIUM }
    , { dlg_box::Actor::MAINC,
        U"\"О-о-о, зрозуміло. Дякую!\"",
        MEDIUM }
    , { dlg_box::Actor::PILOT,
        U"\"Без проблем!\"", 
        MEDIUM }
    , { dlg_box::Actor::NONE, 0, FIX32_ZERO } 
    };

export const dlg_box::TextBlock TB_MINES_JAP[] =
    { { dlg_box::Actor::PILOT, 
        U"「" MAINC_NAME_JAP U"様、気をつけて。",
        MEDIUM }
    , { dlg_box::Actor::PILOT, 
        U"前に地雷があります。たしかに、",
        MEDIUM }
    , { dlg_box::Actor::PILOT,
        U"^-^-遠く^-^-^-か^-ら^-爆発し^-^-^-たほ^-^-う^-^-^-がいいよね。",
        MEDIUM }
    , { dlg_box::Actor::PILOT,
        U"射撃モードを使うのために",
        MEDIUM }
    , { dlg_box::Actor::PILOT,
        U"^4R^1ボタンを押し付けます。",
        MEDIUM }
    , { dlg_box::Actor::PILOT,
        U"そして、",
        MEDIUM }
    , { dlg_box::Actor::PILOT,
        U"十字キ^-^-ーは^-^-カ^-^-ー^-ソル^-を動きます。^-^-^-^-",
        MEDIUM }
    , { dlg_box::Actor::PILOT,
        U"最後に^4A^1ボタンは放つします。」",
        MEDIUM }
    , { dlg_box::Actor::MAINC,
        U"「ええと、",
        MEDIUM }
    , { dlg_box::Actor::MAINC,
        U"このピストルはダメです。",
        MEDIUM }
    , { dlg_box::Actor::MAINC,
        U"例えば、遠い放ってできなくて、",
        MEDIUM }
    , { dlg_box::Actor::MAINC,
        U"クリップは小さくて、",
        MEDIUM }
    , { dlg_box::Actor::MAINC,
        U"リチャージは遅いですよ。」",
        MEDIUM }
    , { dlg_box::Actor::PILOT,
        U"「ところで、爆発は武器の",
        MEDIUM }
    , { dlg_box::Actor::PILOT,
        U"アップデートも散りました。",
        MEDIUM }
    , { dlg_box::Actor::PILOT,
        U"だから、",
        MEDIUM }
    , { dlg_box::Actor::PILOT,
        U"面白い物を見^-つ^-け^-^-る^-^-できます^-^-^-よ^-。^-^-^-^-」^-^-^-^-",
        MEDIUM }
    , { dlg_box::Actor::MAINC,
        U"「分かりました。ありがとう！」^-^-^-",
        MEDIUM }
    , { dlg_box::Actor::PILOT,
        U"「いいえ」",
        MEDIUM }
    , { dlg_box::Actor::NONE, 0, FIX32_ZERO }
    };

export const dlg_box::TextBlock* const TB_MINES[LANG_COUNT] = {
    TB_MINES_ENG,
    TB_MINES_UKR,
    TB_MINES_JAP
};


// Spotting the first missing part

export const dlg_box::TextBlock TB_MAGNET_ENG[] = 
    { { dlg_box::Actor::PILOT, 
        U"\"Lady " MAINC_NAME_ENG U", good news - you're\nnear a precious cargo!\"", 
        MEDIUM }
    , { dlg_box::Actor::MAINC,
        U"\"But the bad news - the container\nis stuck to a magnet.\"",
        MEDIUM }
    , { dlg_box::Actor::PILOT,
        U"\"It's an ultraexpensive premium\ncargo.", 
        MEDIUM }
    , { dlg_box::Actor::PILOT,
        U"Try to cut off the magnet's power.\nWe will cover all the losses.\"", 
        MEDIUM }
    , { dlg_box::Actor::MAINC,
        U"\"Roger, I'm on it.\"",
        MEDIUM }
    , { dlg_box::Actor::NONE, 0, FIX32_ZERO }
    };

export const dlg_box::TextBlock TB_MAGNET_UKR[] = 
    { { dlg_box::Actor::PILOT, 
        U"\"Пані " MAINC_NAME_UKR U", гарна новина - ви\nпоблизу цінного вантажу!\"", 
        MEDIUM }
    , { dlg_box::Actor::MAINC,
        U"\"Погана новина - контейнер\nприлип до електромагніта.\"",
        MEDIUM }
    , { dlg_box::Actor::PILOT,
        U"\"Вантаж преміальний та вкрай\nцінний. ", 
        MEDIUM }
    , { dlg_box::Actor::PILOT,
        U"Попробуйте знеструмити магніт,\nзбитки беремо на себе.\"", 
        MEDIUM }
    , { dlg_box::Actor::MAINC,
        U"\"Зрузуміло, виконую.\"",
        MEDIUM }
    , { dlg_box::Actor::NONE, 0, FIX32_ZERO } 
    };

export const dlg_box::TextBlock TB_MAGNET_JAP[] = 
    { { dlg_box::Actor::PILOT, 
        U"「" MAINC_NAME_JAP U"様、良いニュースは", 
        MEDIUM }
    , { dlg_box::Actor::PILOT, 
        U"近くに大事な貨物があります。」", 
        MEDIUM }
    , { dlg_box::Actor::MAINC,
        U"「でも、悪いニュースは貨物が",
        MEDIUM }
    , { dlg_box::Actor::MAINC,
        U"マグネットに付ついています^-。^-^-」^-^-",
        MEDIUM }
    , { dlg_box::Actor::PILOT,
        U"「貨物は大事なので、", 
        MEDIUM }
    , { dlg_box::Actor::PILOT,
        U"電気を消しなければならない。", 
        MEDIUM }
    , { dlg_box::Actor::PILOT,
        U"もちろん、", 
        MEDIUM }
    , { dlg_box::Actor::PILOT,
        U"会社はカバ^-^-ー^-^-さ^-^-せ^-て^-も^-^-ら^-^-います^-^-。^-^-^-」^-^-^-", 
        MEDIUM }
    , { dlg_box::Actor::MAINC,
        U"「よし、俺に任せろ。」",
        MEDIUM }
    , { dlg_box::Actor::NONE, 0, FIX32_ZERO } 
    };


export const dlg_box::TextBlock* const TB_MAGNET[LANG_COUNT] = {
    TB_MAGNET_ENG,
    TB_MAGNET_UKR,
    TB_MAGNET_JAP
};


// Computer's key tutorial (green key)

export const dlg_box::TextBlock TB_CKEY_TUT_ENG[] = 
    { { dlg_box::Actor::COMPUTER, 
        U"\"ACCESS DENIED! Please insert\na ^3green^1 keycard.\"", 
        MEDIUM }
    , { dlg_box::Actor::PILOT,
        U"\"Lady " MAINC_NAME_ENG U", keycard must be\nsomewhere nearby.\"",
        MEDIUM }
    , { dlg_box::Actor::MAINC,
        U"\"Gotcha.\"",
        MEDIUM }
    , { dlg_box::Actor::NONE, 0, FIX32_ZERO }
    };

export const dlg_box::TextBlock TB_CKEY_TUT_UKR[] = 
    { { dlg_box::Actor::COMPUTER, 
        U"\"ЗАБОРОНЕНО! Для доступу\nнеобхідна ^3зелена^1 ключ-карта.\"^-^-", 
        MEDIUM }
    , { dlg_box::Actor::PILOT,
        U"\"Пані " MAINC_NAME_UKR U", ключ-карта десь\nпоруч.\"",
        MEDIUM }
    , { dlg_box::Actor::MAINC,
        U"\"Ясно. Шукаємо.\"",
        MEDIUM }
    , { dlg_box::Actor::NONE, 0, FIX32_ZERO } 
    };

export const dlg_box::TextBlock TB_CKEY_TUT_JAP[] = 
    { { dlg_box::Actor::COMPUTER, 
        U"「アクセスが拒否されました！", 
        MEDIUM }
    , { dlg_box::Actor::COMPUTER, 
        U"^3緑^1のキ^-^-^-ー^-^-カ^-^-^-ー^-^-^-ドをお使い^-^-^-く^-^-^-ださい^-。^-^-^-^-」^-^-^-^-^-", 
        MEDIUM }
    , { dlg_box::Actor::PILOT,
        U"「" MAINC_NAME_JAP U"様、キーカードは",
        MEDIUM }
    , { dlg_box::Actor::PILOT,
        U"近くにあるかな？」",
        MEDIUM }
    , { dlg_box::Actor::MAINC,
        U"「分か^-^-^-り^-^-ま^-し^-^-た^-。^-^-見^-つ^-け^-ま^-^-し^-^-ょ^-^-う^-^-。^-^-」^-^-^-",
        MEDIUM }
    , { dlg_box::Actor::NONE, 0, FIX32_ZERO } 
    };


export const dlg_box::TextBlock* const TB_CKEY_TUT[LANG_COUNT] = {
    TB_CKEY_TUT_ENG,
    TB_CKEY_TUT_UKR,
    TB_CKEY_TUT_JAP
};


// Computer plays the password melody

export const dlg_box::TextBlock TB_CPASSM_ENG[] = 
    { { dlg_box::Actor::COMPUTER, 
        U"\"PLAYING THE PASSWORD...\"",
        MEDIUM }
    , { dlg_box::Actor::NONE, 0, FIX32_ZERO } 
    };

export const dlg_box::TextBlock TB_CPASSM_UKR[] = 
    { { dlg_box::Actor::COMPUTER, 
        U"\"ВІДТВОРЮЮ ПАРОЛЬ...\"",
        MEDIUM }
    , { dlg_box::Actor::NONE, 0, FIX32_ZERO } 
    };

export const dlg_box::TextBlock TB_CPASSM_JAP[] = 
    { { dlg_box::Actor::COMPUTER, 
        U"「パスワードをプレイバック…」^-^-^-^-",
        MEDIUM }
    , { dlg_box::Actor::NONE, 0, FIX32_ZERO } 
    };

export const dlg_box::TextBlock* const TB_CPASSM[LANG_COUNT] = {
    TB_CPASSM_ENG,
    TB_CPASSM_UKR,
    TB_CPASSM_JAP
};


// Computer access (blue key)

export const dlg_box::TextBlock TB_BLUE_KEY_ENG[] = 
    { { dlg_box::Actor::COMPUTER, 
        U"\"ACCESS DENIED! Please insert\n^5blue^1 keycard.\"", 
        MEDIUM }
    , { dlg_box::Actor::NONE, 0, FIX32_ZERO }
    };

export const dlg_box::TextBlock TB_BLUE_KEY_UKR[] = 
    { { dlg_box::Actor::COMPUTER, 
        U"\"ЗАБОРОНЕНО! Для доступу\nнеобхідна ^5синя^1 ключ-карта.\"^-^-", 
        MEDIUM }
    , { dlg_box::Actor::NONE, 0, FIX32_ZERO } 
    };

export const dlg_box::TextBlock TB_BLUE_KEY_JAP[] = 
    { { dlg_box::Actor::COMPUTER, 
        U"「アクセスが拒否されました！", 
        MEDIUM }
    , { dlg_box::Actor::COMPUTER, 
        U"^5青い^1キ^-^-^-ー^-^-カ^-^-^-ー^-^-^-ドをお使い^-^-^-く^-^-^-ださい^-。^-^-^-^-」^-^-^-^-^-", 
        MEDIUM }
    , { dlg_box::Actor::NONE, 0, FIX32_ZERO } 
    };

export const dlg_box::TextBlock* const TB_BLUE_KEY[LANG_COUNT] = {
    TB_BLUE_KEY_ENG,
    TB_BLUE_KEY_UKR,
    TB_BLUE_KEY_JAP
};


// Magnets OFF

export const dlg_box::TextBlock TB_MAGNET_OFF_ENG[] = 
    { { dlg_box::Actor::COMPUTER, 
        U"\"MAGNET OFF!\"", 
        MEDIUM }
    , { dlg_box::Actor::PILOT, 
        U"\"Nice! We're almost there!\"", 
        MEDIUM }
    , { dlg_box::Actor::NONE, 0, FIX32_ZERO }
    };

export const dlg_box::TextBlock TB_MAGNET_OFF_UKR[] = 
    { { dlg_box::Actor::COMPUTER, 
        U"\"МАГНІТ ВИМКНЕНО!\"", 
        MEDIUM }
    , { dlg_box::Actor::PILOT, 
        U"\"Чудово! Ми майже там!\"", 
        MEDIUM }
    , { dlg_box::Actor::NONE, 0, FIX32_ZERO } 
    };

export const dlg_box::TextBlock TB_MAGNET_OFF_JAP[] = 
    { { dlg_box::Actor::COMPUTER, 
        U"「マグネットがオフ！」",
        MEDIUM }
    , { dlg_box::Actor::PILOT, 
        U"「よし、もうすぐ」", 
        MEDIUM }
    , { dlg_box::Actor::NONE, 0, FIX32_ZERO } 
    };

export const dlg_box::TextBlock* const TB_MAGNET_OFF[LANG_COUNT] = {
    TB_MAGNET_OFF_ENG,
    TB_MAGNET_OFF_UKR,
    TB_MAGNET_OFF_JAP
};


// Final dialogue

export const dlg_box::TextBlock TB_F00_END_ENG[] =
    {  { dlg_box::Actor::PILOT,
        U"\"Lady " MAINC_NAME_ENG U", congratulations!\"",
        MEDIUM }
    ,  { dlg_box::Actor::MAINC,
        U"\"...",
        MEDIUM }
    ,  { dlg_box::Actor::MAINC,
        U"...Hm, the package is damaged.\nI'll check the condition...\"",
        MEDIUM }
    ,  { dlg_box::Actor::PILOT,
        U"\"Lady " MAINC_NAME_ENG U", no, you can't, it's\na premium package...\"",
        MEDIUM }
    ,  { dlg_box::Actor::MAINC,
        U"\"...Alright.",
        MEDIUM }
    ,  { dlg_box::Actor::MAINC,
        U"Oops, it's turning on!\"",
        MEDIUM }
    , { dlg_box::Actor::COMPUTER, 
        U"\"Princess " MAINC_NAME_ENG U", I'm the one who\nplanted the bomb.", 
        MEDIUM }
    , { dlg_box::Actor::COMPUTER, 
        U"Your cargo is very dangerous.\nDo not trust anyone. ", 
        MEDIUM }
    , { dlg_box::Actor::COMPUTER, 
        U"Destroy the cargo as soon as\npossible. ", 
        MEDIUM }
    , { dlg_box::Actor::COMPUTER, 
        U"I repeat, don't trust the company\nand your co-workers!\"", 
        MEDIUM }
    ,  { dlg_box::Actor::MAINC,
        U"\". . .\"",
        MEDIUM }
    ,  { dlg_box::Actor::PILOT,
        U"\". . .\"",
        MEDIUM }
    , { dlg_box::Actor::COMPUTER, 
        U"\"TO BE CONTINUED...\"", 
        MEDIUM }
    , { dlg_box::Actor::NONE, 0, FIX32_ZERO }
    };

export const dlg_box::TextBlock TB_F00_END_UKR[] =
    {  { dlg_box::Actor::PILOT,
        U"\"Пані " MAINC_NAME_UKR U", поздоровляю!\"",
        MEDIUM }
    ,  { dlg_box::Actor::MAINC,
        U"\"...",
        MEDIUM }
    ,  { dlg_box::Actor::MAINC,
        U"...Хм, упаковка пошкоджена.\nПеревіряю цілісність...\"",
        MEDIUM }
    ,  { dlg_box::Actor::PILOT,
        U"\"Пані " MAINC_NAME_UKR U", ні, не можна, це\nпреміальна посилка...\"",
        MEDIUM }
    ,  { dlg_box::Actor::MAINC,
        U"\"...Все в порядку.",
        MEDIUM }
    ,  { dlg_box::Actor::MAINC,
        U"Ой, воно включається!\"",
        MEDIUM }
    , { dlg_box::Actor::COMPUTER, 
        U"\"Принцеса " MAINC_NAME_UKR U", я - це той хто\nзаклав вибухівку.", 
        MEDIUM }
    , { dlg_box::Actor::COMPUTER, 
        U"Вантаж вашого корабля вкрай\nнебезпечний.", 
        MEDIUM }
    , { dlg_box::Actor::COMPUTER, 
        U"Не вірте нікому. Пошвидше\nзнищіть вантаж.", 
        MEDIUM }
    , { dlg_box::Actor::COMPUTER, 
        U"Повторюю, не вірте компанії та\nвашим колегам!\"", 
        MEDIUM }
    ,  { dlg_box::Actor::MAINC,
        U"\". . .\"",
        MEDIUM }
    ,  { dlg_box::Actor::PILOT,
        U"\". . .\"",
        MEDIUM }
    , { dlg_box::Actor::COMPUTER, 
        U"\"ДАЛІ БУДЕ...\"", 
        MEDIUM }
    , { dlg_box::Actor::NONE, 0, FIX32_ZERO } 
    };

export const dlg_box::TextBlock TB_F00_END_JAP[] =
    {  { dlg_box::Actor::PILOT,
        U"「" MAINC_NAME_JAP U"様、おめでとう！」",
        MEDIUM }
    ,  { dlg_box::Actor::MAINC,
        U"「．．．",
        MEDIUM }
    ,  { dlg_box::Actor::MAINC,
        U"…ええと",
        MEDIUM }
    ,  { dlg_box::Actor::MAINC,
        U"包みがすこし損じました。",
        MEDIUM }
    ,  { dlg_box::Actor::MAINC,
        U"ちょっとチェキします…」",
        MEDIUM }
    ,  { dlg_box::Actor::PILOT,
        U"「" MAINC_NAME_JAP U"様、ダメだ。",
        MEDIUM }
    ,  { dlg_box::Actor::PILOT,
        U"これは大事な貨物は…」",
        MEDIUM }
    ,  { dlg_box::Actor::MAINC,
        U"「大丈夫…",
        MEDIUM }
    ,  { dlg_box::Actor::MAINC,
        U"何？ 点けられていた！」",
        MEDIUM }
    , { dlg_box::Actor::COMPUTER, 
        U"「" MAINC_NAME_JAP U"様、爆発は私のせいです。", 
        MEDIUM }
    , { dlg_box::Actor::COMPUTER, 
        U"この貨物はとても危ないです。", 
        MEDIUM }
    , { dlg_box::Actor::COMPUTER, 
        U"誰を信じない。", 
        MEDIUM }
    , { dlg_box::Actor::COMPUTER, 
        U"はやく貨物が壊せ！", 
        MEDIUM }
    , { dlg_box::Actor::COMPUTER, 
        U"もう一度言う", 
        MEDIUM }
    , { dlg_box::Actor::COMPUTER, 
        U"会社と会員を信じない！」", 
        MEDIUM }
    ,  { dlg_box::Actor::MAINC,
        U"「．．．」",
        MEDIUM }
    ,  { dlg_box::Actor::PILOT,
        U"「．．．」",
        MEDIUM }
    , { dlg_box::Actor::COMPUTER, 
        U"「つづく．．．」", 
        MEDIUM }
    , { dlg_box::Actor::NONE, 0, FIX32_ZERO } 
    };

export const dlg_box::TextBlock* const TB_F00_END[LANG_COUNT] = {
    TB_F00_END_ENG,
    TB_F00_END_UKR,
    TB_F00_END_JAP
};

} // namespace factory_00


// ------------------------------ Factory 01 ------------------------------- //

namespace factory_01 {

// Thank you window

export const dlg_box::TextBlock TB_THANK_YOU_ENG[] = 
    { { dlg_box::Actor::COMPUTER, 
        U"Thank you for playing the\ndemo!", 
        MEDIUM }
    , { dlg_box::Actor::COMPUTER, 
        U"Made for GBA Jam 2026.\nby Pavlo Savchuk (aka zegalur).^-^-^-", 
        MEDIUM }
    , { dlg_box::Actor::NONE, 0, FIX32_ZERO }
    };

export const dlg_box::TextBlock TB_THANK_YOU_UKR[] = 
    { { dlg_box::Actor::COMPUTER, 
        U"Дякую що пройшли демо!", 
        MEDIUM }
    , { dlg_box::Actor::COMPUTER, 
        U"Зроблено для GBA Jam 2026.\nПавло Савчук (zegalur).", 
        MEDIUM }
    , { dlg_box::Actor::NONE, 0, FIX32_ZERO } 
    };

export const dlg_box::TextBlock TB_THANK_YOU_JAP[] = 
    { { dlg_box::Actor::COMPUTER, 
        U"デモを終わってありがとう。", 
        MEDIUM }
    , { dlg_box::Actor::COMPUTER, 
        U"GBA Jam 2026 に作かれました。", 
        MEDIUM }
    , { dlg_box::Actor::COMPUTER, 
        U"サ^-ヴ^-チ^-^-ュ^-^-ク^-^-^-　パ^-ウロ（zegalur）^-^-。^-^-^-", 
        MEDIUM }
    , { dlg_box::Actor::NONE, 0, FIX32_ZERO } 
    };

export const dlg_box::TextBlock* const TB_THANK_YOU[LANG_COUNT] = {
    TB_THANK_YOU_ENG,
    TB_THANK_YOU_UKR,
    TB_THANK_YOU_JAP
};




} // namespace factory_01

} // namespace map

} // namespace game
