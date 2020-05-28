#include <SDL2/SDL.h> //apt-get install libsdl1.2-dev
#include "compte.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sdl.h"
#include "blockchain.h"
#include <SDL2/SDL_ttf.h>   //apt-get install libsdl2-ttf-dev
#include <SDL2/SDL_image.h> //apt-get install libsdl2-image-dev

void init_global(Data* Bottle)
{
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
    Bottle->Main_Window = SDL_CreateWindow("Bottle", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1374, 775, 0);
    Bottle->Main_Renderer = SDL_CreateRenderer(Bottle->Main_Window, -1, SDL_RENDERER_ACCELERATED);

    strcpy(Bottle->image,"images/menu.bmp");
    Bottle->menu_Texture = init_texture(Bottle, Bottle->menu_Texture);
    
    strcpy(Bottle->image,"images/compte.bmp");
    Bottle->compte_Texture = init_texture(Bottle, Bottle->compte_Texture);

    strcpy(Bottle->image,"images/connexion.bmp");
    Bottle->connexion_Texture = init_texture(Bottle, Bottle->connexion_Texture);

    strcpy(Bottle->image,"images/messagerie.bmp");
    Bottle->messagerie_Texture = init_texture(Bottle, Bottle->messagerie_Texture);

    strcpy(Bottle->image,"images/destinataire.bmp");

    Bottle->destinataire_Texture = init_texture(Bottle, Bottle->destinataire_Texture);
    
}


SDL_Texture* init_texture(Data* Bottle, SDL_Texture* texture)
{
    int textureW, textureH;
    Bottle->Loading_Surf = SDL_LoadBMP(Bottle->image);
    texture = SDL_CreateTextureFromSurface(Bottle->Main_Renderer, Bottle->Loading_Surf);
    SDL_QueryTexture(texture, NULL, NULL, &textureW,&textureH);
    SDL_FreeSurface(Bottle->Loading_Surf); /* we got the texture now -> free surface */
    return texture;
}



void Write(Data* Bottle, Message* mes)
{
    TTF_Font *police = TTF_OpenFont("./lsans.ttf",mes->tailleP);
    if(police == NULL)
    {
        printf("%s\n", TTF_GetError());
    }

    mes->texteSurface = TTF_RenderText_Blended(police, mes->texte,mes->couleur);
    if(mes->texteSurface == NULL)
    {
        printf("%s\n", TTF_GetError());
    }
    
    mes->texture = SDL_CreateTextureFromSurface(Bottle->Main_Renderer, mes->texteSurface);
    if(mes->texture == NULL)
    {
        printf("%s", SDL_GetError());
    }
    SDL_QueryTexture(Bottle->message, NULL, NULL, &mes->textRect.w, &mes->textRect.h);
    SDL_FreeSurface(mes->texteSurface);

    SDL_RenderCopy(Bottle->Main_Renderer, mes->texture, NULL, &mes->textRect);
    SDL_RenderPresent(Bottle->Main_Renderer);
}


void SDL_Dest(Data *donnee)//Détruit les texture le rendu et la fenêtre
{
    SDL_DestroyTexture(donnee->menu_Texture);
    SDL_DestroyTexture(donnee->compte_Texture);
    SDL_DestroyTexture(donnee->connexion_Texture);
    SDL_DestroyTexture(donnee->destinataire_Texture);
    SDL_DestroyTexture(donnee->messagerie_Texture);
    SDL_DestroyRenderer(donnee->Main_Renderer);
    SDL_DestroyWindow(donnee->Main_Window);
}

void DisplayUsers(TABID* TabID, Data* Bottle)  //Retourne le choix
{
    int taille = TabID->taille;
    
    SDL_RenderCopy(Bottle->Main_Renderer, Bottle->destinataire_Texture, NULL, NULL);
    SDL_RenderPresent(Bottle->Main_Renderer);
    Message* mes = (Message*)malloc(sizeof(Message));
    mes->textRect.x = 610;      //La position du message
    mes->textRect.y = 335;
    mes->textRect.h = 50;       //Choisir la longeur du message
    mes->tailleP = 60;
    mes->couleur.b = 0;
    mes->couleur.a = 0;
    mes->couleur.r = 0;
    mes->couleur.g = 0;
    for(int i = 0; i<taille; i++)
    {
        mes->textRect.w = 25*strlen(TabID->ID[i].username);
        mes->textRect.y += mes->textRect.h;
        
        strcpy(mes->texte,TabID->ID[i].username );
        Write(Bottle, mes);
    }
    free(mes);
}

void DisplayMessagerie(char* exp, char* dest, Data* Bottle)
{
    Message* mes = (Message*)malloc(sizeof(Message));
    
    mes->textRect.y = 165;
    mes->textRect.h = 40;    //Choisir la largeur du message
    mes->tailleP = 40;
    mes->couleur.b = 0;
    mes->couleur.a = 0;
    mes->couleur.r = 0;
    mes->couleur.g = 0;

    mes->textRect.w = 25*strlen(exp);
    mes->textRect.x = 535-mes->textRect.w;     //La position du message    
    strcpy(mes->texte, exp);

    Write(Bottle, mes);
    mes->textRect.w = 25*strlen(dest);
    mes->textRect.x = 820;
    strcpy(mes->texte, dest);
    Write(Bottle, mes);

    mes->textRect.x = 60;
    struct bloc *currentbloc = Genesis->premier;
    int cmpt = 0;               //compteur du nombre de messages affiché et ne doit pas dépasser NbMessages
    mes->textRect.y = 590;
    mes->textRect.h = 25;       //Choisir la largeur du message
    mes->tailleP = 20;
    while(cmpt <= NbMessages && currentbloc != NULL)
    {   
        if((strcmp(currentbloc->donnee->exp, exp) == 0 && strcmp(currentbloc->donnee->dest, dest) == 0) || (strcmp(currentbloc->donnee->dest, exp) == 0 && strcmp(currentbloc->donnee->exp, dest) == 0))    //On test qu'on a bien les bon exp et dest
        {
            cmpt++;
            strcpy(mes->texte, "[");
            strcat(mes->texte, currentbloc->donnee->date);
            strcat(mes->texte, "]");
            strcat(mes->texte, " ");
            strcat(mes->texte, currentbloc->donnee->exp);
            strcat(mes->texte, " : ");
            strcat(mes->texte, currentbloc->donnee->message);
            mes->textRect.w = 12*strlen(mes->texte);
            mes->textRect.y -= mes->textRect.h ; 
            Write(Bottle, mes);
        }
        currentbloc = currentbloc->lien;          
    }
    free(mes);

    
}

bool IsValidUsername(char* username, TABID* TabID)
{
    int taille = TabID->taille;

    for(int i = 0; i<taille; i++)
    {
        if(strcmp(username, TabID->ID[i].username) == 0)
        {
            return true;
        }
    }
    return false;
}

void* MenuThread(void* arg)
{
    SDL_Rect Connect;
    SDL_Rect Compte;
    Connect.x = 600; Connect.y = 300; Connect.h = 37; Connect.w = 360;
    Compte.x = 600; Connect.y = 425; Connect.h = 37; Connect.w = 690;    
    while(1)
    {
        while(Etat != ETAT_MENU);
        DisplayBackground(Bottle.menu_Texture);
        while(Etat == ETAT_MENU)
        {
            if(SDL_PointInRect(&Input.PointPressed, &Connect)) //Se connecter
            {
                ResetInput();
                printf("connection\n");
                //Etat = ETAT_CONNECTION;
            }
            if(SDL_PointInRect(&Input.PointPressed, &Compte))   //Créer Compte
            {
                ResetInput();
                printf("compte\n");
                //Etat = ETAT_COMPTE;
            }
        }
    }
}

void* CompteConnecterThread(void* arg)
{
    SDL_Rect BarreUsername;
    BarreUsername.x = 765; BarreUsername.y = 305; BarreUsername.h = 50; BarreUsername.w = 535;
    SDL_Rect BarrePassword;
    BarrePassword.x = 765; BarrePassword.y = 425; BarrePassword.h = 50; BarrePassword.w = 535;
    SDL_Rect Retour;
    Retour.x = 1200; Retour.y = 523; Retour.h = 30; Retour.w = 100;
    struct Identifiant Id;
    ResetId(&Id);
    Message mesUsername; mesUsername.textRect.x = 770; mesUsername.textRect.y = 310; mesUsername.textRect.h = 35; mesUsername.textRect.w = 25; 
    mesUsername.tailleP = 30; mesUsername.couleur.b = 0; mesUsername.couleur.a = 0; mesUsername.couleur.r = 0; mesUsername.couleur.g = 0;
    Message mesPassword; mesUsername.textRect.x = 770; mesUsername.textRect.y = 435; mesUsername.textRect.h = 35; mesUsername.textRect.w = 25; 
    mesUsername.tailleP = 30; mesUsername.couleur.b = 0; mesUsername.couleur.a = 0; mesUsername.couleur.r = 0; mesUsername.couleur.g = 0;
    while(1)
    {
        while(Etat == ETAT_CONNECTION || Etat == ETAT_COMPTE)
        {
            if(Etat == ETAT_CONNECTION)                         //Affichage du fond
                DisplayBackground(Bottle.connexion_Texture);
            else
                DisplayBackground(Bottle.compte_Texture);

            BarreSaisie(&mesUsername, Id.username, &BarreUsername); //Barre de saisi Username et Password
            BarreSaisie(&mesPassword, Id.password, &BarrePassword);

            if(SDL_PointInRect(&Input.PointPressed, &Retour))   //On appuie sur Retour
            {
                ResetInput();
                Etat = ETAT_MENU;
            }
            if(Input.BouttonClavier == SDLK_RETURN)     //On appuie sur entrée
            {
                if(Etat == ETAT_CONNECTION && checkExistenceElementInTabID(&TabID, &Id))    //Cas où on est dans Se connecter on vérifie que les identifiants rentrés sont correct et on passe à la messagerie
                {
                    ResetInput();
                    strcpy(expediteur, Id.username);      //On met à jour la variable expediteur
                    Etat = ETAT_MESSAGERIE;
                }
                else if (Etat = ETAT_COMPTE && !checkExistenceElementInTabID(&TabID, &Id))  //Cas où on est dans Créer un compte, on vérifie que les Ids rentrés n'existent pas déjà et on crée le compte et on revient au menu
                {
                    SignUp(&TabID, &Id);
                    SaveTabID(&TabID, FileNameID);
                    ResetInput();
                    Etat = ETAT_MENU;
                }
                mesUsername.textRect.x = 770; mesUsername.textRect.y = 310; //On reset la barre de saisie Username et Password
                mesUsername.textRect.x = 770; mesUsername.textRect.y = 435;
                ResetId(&Id);                   
            }           
        }
    }
}

void* DestinataireThread(void* arg)
{
    char User[MAX_WORD_LENGHT];
    Message mes;                //Message à afficher mais on le fait lettre par lettre donc != Texte
    mes.textRect.x = 83;        //Position de la barre de saisie
    mes.textRect.y = 220;
    mes.textRect.h = 35;        //Choisir la longeur du message (Une lettre)
    mes.textRect.w = 25;        //Choisir la largeur du message
    mes.tailleP = 30;
    mes.couleur.b = 0; mes.couleur.a = 0; mes.couleur.r = 0; mes.couleur.g = 0;
    SDL_Rect Barre;
    Barre.x = 83; Barre.y = 220; Barre.h = 60; Barre.w = 1310;  //Ce ne sont pas les bonnes valeurs il faut les changer
    strcpy(User, "");
    while(1)
    {
        while(Etat == ETAT_DESTINATAIRE)
        {
            DisplayBackground(Bottle.destinataire_Texture);
            DisplayUsers(&TabID, &Bottle);
            
            BarreSaisie(&mes, User, &Barre);

            if(Input.BouttonClavier == SDLK_RETURN)
            {
                if(IsValidUsername(User, &TabID))
                {   
                    ResetInput();
                    strcpy(destinataire, User);
                    Etat = ETAT_MESSAGERIE;
                }
                strcpy(User, "");
                mes.textRect.x = 83;
                mes.textRect.y = 220;
            }
        }
    }
}

void* MessagerieThread(void* arg)
{
    Message mes;                //Message à afficher mais on le fait lettre par lettre donc != Texte
    mes.textRect.x = 30;        //Position de la barre de saisie
    mes.textRect.y = 670;
    mes.textRect.h = 35;        //Choisir la longeur du message (Une lettre)
    mes.textRect.w = 25;        //Choisir la largeur du message
    mes.tailleP = 30;
    mes.couleur.b = 0; mes.couleur.a = 0; mes.couleur.r = 0; mes.couleur.g = 0;
    SDL_Rect Barre;
    Barre.x = 30; Barre.y = 670; Barre.h = 60; Barre.w = 1310;
    SDL_Rect Retour;
    Retour.x = 1240; Retour.y = 730; Retour.h = 30; Retour.w = 100;
    donnee DataBloc;            //Donnée du bloc à ajouter
    strcpy(DataBloc.exp, expediteur);
    strcpy(DataBloc.dest, destinataire);
    strcpy(DataBloc.message, "");
    while(1)
    {
        while(Etat == ETAT_MESSAGERIE)
        {   
            DisplayBackground(Bottle.messagerie_Texture);   //Affichage du fond
            DisplayMessagerie(DataBloc.exp, DataBloc.dest, &Bottle);

            BarreSaisie(&mes, DataBloc.message, &Barre);    //Barre de saisie des messages à envoyer

            if(SDL_PointInRect(&Input.PointPressed, &Retour))   //On appuie sur Retour
            {
                ResetInput();
                Etat = ETAT_CONNECTION;
            }
            if(Input.BouttonClavier == SDLK_RETURN) //Envoie du message
            {
                Input.BouttonClavier = SDLK_UNKNOWN;
                getTime(DataBloc.date);
                ajout_block(&DataBloc);
                /*On save et Load 2 fois car la blockchain a été codé comme une pile et donc save inverse le sens de la pile*/
                SaveBlockChain(FileNameBC);
                initGenesis();
                LoadBlockChainFromFile2(FileNameBC);
                SaveBlockChain(FileNameBC);
                initGenesis();
                LoadBlockChainFromFile2(FileNameBC);

                strcpy(DataBloc.message, "");   //On reset les données de la barre de saisie et les inputs
                ResetInput();
                mes.textRect.x = 30;
                mes.textRect.y = 670;
            }
        }
    }
}

void BarreSaisie(Message* mes, char texte[MAX_WORD_LENGHT], SDL_Rect* Barre)
{
    while(Input.BouttonClavier != SDLK_RETURN && SDL_PointInRect(&Input.PointPressed, Barre))
    {
        if(Input.BouttonClavier != SDLK_UNKNOWN && strlen(texte) < MAX_WORD_LENGHT && strlen(SDL_GetKeyName(Input.BouttonClavier)) <= 1)
        {
            strcat(texte,SDL_GetKeyName(Input.BouttonClavier));
            strcpy(mes->texte, SDL_GetKeyName(Input.BouttonClavier));
            mes->textRect.x += 25;
            Write(&Bottle, mes);
            Input.BouttonClavier = SDLK_UNKNOWN;
        }
    }
    //Faire le cas où on appui sur espace
}

void DisplayBackground(SDL_Texture* Texture)    //Affiche Texture en fond
{
    SDL_RenderCopy(Bottle.Main_Renderer, Texture, NULL, NULL);
    SDL_RenderPresent(Bottle.Main_Renderer);
}

void ResetInput()   //Reset Input à 0
{
    Input.PointPressed.x = 0;
    Input.PointPressed.y = 0;
    Input.BouttonClavier = SDLK_UNKNOWN;
}

void ResetId(struct Identifiant* Id)    //Reset les identifiants
{
    strcpy(Id->username, "");
    strcpy(Id->password, "");
}