/// BiCycle management system
/// version - 1.4
#include <ctype.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>

#define MAX_PASSWORD_LENGTH 100
#define MAX_USERNAME_LENGTH 100

/// global variables
int choice;

/// DB variables (pointers)
FILE *bicycleInfoDB, *adminDB, *studentDB, *rentalHistoryDB;

/// Structures for developer info
typedef struct DeveloperInfo
{
    int serial;
    char developer_name[1005], developer_id[1005], developer_section[1005];
} developerInfo;

/// Structure for admin section
typedef struct AdminNode
{
    char username[MAX_USERNAME_LENGTH];
    char password[MAX_PASSWORD_LENGTH];
    char role[MAX_USERNAME_LENGTH];
    struct AdminUserNode *next;
} adminUserNode;

/// Structure for admin section
typedef struct SystemSettingInfo
{
    int maintenance_mode;
    int admin_login;
    char loggedin_admin_username[MAX_USERNAME_LENGTH];
    int student_login;
    char loggedin_student_username[MAX_USERNAME_LENGTH];
} SystemSettingInfo;

typedef struct SystemSetting
{
    SystemSettingInfo info;
    struct SystemSetting *next;
} systemSettingNode;


/// Structure for student section
typedef struct StudentNode
{
    char username[MAX_USERNAME_LENGTH];
    char password[MAX_PASSWORD_LENGTH];
    struct StudentUserNode *next;
} studentUserNode;

/// Structure to represent a bicycle
typedef struct BicycleInfo
{
    int id;
    int inventory_quantity;
    int rent_quantity;
    char brand[50];
    char model[50];
    double rental_cost;
} bicycleInfo;

typedef struct Bicycle
{
    bicycleInfo info;
    struct Bicycle *next;
} bicycle;


/// Structure for Rental History
typedef struct RentalHistory
{
    int rental_id;
    int bicycle_id;
    int rental_duration;
    double rental_cost;
    char brand[100];
    char model[100];
    char username[100];
    char transation_id[100];
    char payment_method[100];
} RentalHistory;

typedef struct RentalHistoryNode
{
    RentalHistory history;
    struct RentalHistoryNode *next;
} RentalHistoryNode;



/// System Setting Head
systemSettingNode *systemSettingList = NULL;

/// Bicycle List head
bicycle *bicycleList = NULL;

/// Admin Node Head
adminUserNode *adminUserList = NULL;

/// Student Node Head
studentUserNode *studentUserList = NULL;

/// Rental History Node
RentalHistoryNode *RentalHistoryList = NULL;



/// function prototypes
/// main menu, student related functions
void showName(char *name);
void mainMenu();
void studentIntermediateMenu();
void studentLogin();
void studentSignup();
studentUserNode* createStudentUserNode(char *username, char *password, int readValidity);
void insertStudentUser(studentUserNode *head, char *username, char *password, int readValidity);
int authenticateStudent(studentUserNode *studentUserList, char *username, char *password);
void studentMainMenu(char *name);

/// administrative functions and password hashing
void adminLogin();
void manageAdmin(adminUserNode *adminUserList, char *username);
adminUserNode* createAdminUserNode(char *username, char *password, int readValidity);
void addAdmin(adminUserNode *adminUserList, char *username);
int updateAdmin(adminUserNode *adminUserList, char *keyname, char *username);
int removeAdmin(adminUserNode *adminUserList, char *keyname);
int viewAdmin(adminUserNode *adminUserList, char *username);
void insertAdminUser(adminUserNode *head, char *username, char *password,int readValidity);
void changeAdminPassword(adminUserNode *adminUserList, char *username);
void adminMainMenu(char *name);
//int viewUser(studentUserNode *studentUserList, char *username);
void initSystemSetting(systemSettingNode *systemSettingList);
void hashPassword(char *password);
void maintenanceBreak(systemSettingNode *systemSettingList);

/// renting, transaction, update functions for bicycle
bicycle* createBicycle(bicycleInfo info);
int addBicycle(bicycle* head, bicycleInfo info, int readValidity);
int updateCycle(bicycle *head, int cycleId,char *name);
int removeCycle(bicycle *head, int cycleId);
int getUniqueBicycleId(bicycle *bicycleList);
int displayBicycles(bicycle* head);
int rentCycle(bicycle *bicycleList, int cycleId, char *username);
int paymentForRent(bicycle *current, double rental_cost);
int pay_with_bkash_or_nagad(int payment_choice, double rental_cost);
void generateTransactionID(char *transactionID, int length);
int getUniqueRentalId(RentalHistoryNode *RentalHistoryList);
int viewRentalHistory(RentalHistoryNode *RentalHistoryList, char *username);

/// rules, developers section
void rulesRegulations();
void aboutDevelopers();

int main()
{

    srand(time(NULL));

    bicycleList = (bicycle*) malloc(sizeof(bicycle));
    bicycleList -> next = NULL;

    adminUserList =(adminUserNode*) malloc(sizeof(adminUserNode));
    adminUserList -> next = NULL;

    studentUserList =(studentUserNode*) malloc(sizeof(studentUserNode));
    studentUserList -> next = NULL;

    systemSettingList =(systemSettingNode*) malloc(sizeof(systemSettingNode));
    systemSettingList -> next = NULL;
    initSystemSetting(systemSettingList);

    RentalHistoryList =(RentalHistoryNode*) malloc(sizeof(RentalHistoryNode));
    RentalHistoryList -> next = NULL;

    /// Read from DB

    /// Bicycle Info DB
    bicycleInfoDB = fopen("bicycle_info.txt", "r");
    bicycleInfo info;
    while (fscanf(bicycleInfoDB, "%d %s %s %lf %d\n", &info.id, info.brand, info.model, &info.rental_cost, &info.inventory_quantity) != EOF) {
        addBicycle(bicycleList, info, 1);
    }

    fclose(bicycleInfoDB);

    /// Admin Info DB
    adminDB = fopen("admin_info.txt", "r");
    char username[MAX_USERNAME_LENGTH], password[MAX_PASSWORD_LENGTH];
    while (fscanf(bicycleInfoDB, "%s %s\n", username, password) != EOF) {
        insertAdminUser(adminUserList, username, password, 1);
    }

    fclose(adminDB);

    /// Admin Info DB
    studentDB = fopen("student_info.txt", "r");
    while (fscanf(studentDB, "%s %s\n", username, password) != EOF) {
        insertStudentUser(studentUserList, username, password, 1);
    }

    fclose(studentDB);

    /// Rental History read

    rentalHistoryDB = fopen("rental_history.txt", "r");
    RentalHistory history;
    while(fscanf(rentalHistoryDB, "%d %d %d %lf %s %s %s %s %s\n", &history.rental_id, &history.bicycle_id, &history.rental_duration, &history.rental_cost, history.brand, history.model, history.username, history.transation_id, history.payment_method) != EOF){
        RentalHistoryNode *rentalHistoryNode = (RentalHistoryNode*) malloc(sizeof(RentalHistoryNode));

        rentalHistoryNode ->history.rental_id = history.rental_id;
        rentalHistoryNode ->history.bicycle_id = history.bicycle_id;
        rentalHistoryNode ->history.rental_duration = history.rental_duration;
        rentalHistoryNode ->history.rental_cost = history.rental_cost;
        strcpy(rentalHistoryNode ->history.brand, history.brand);
        strcpy(rentalHistoryNode ->history.brand, history.brand);
        strcpy(rentalHistoryNode ->history.model, history.model);
        strcpy(rentalHistoryNode ->history.username, history.username);

        strcpy(rentalHistoryNode ->history.transation_id, history.transation_id);
        strcpy(rentalHistoryNode ->history.payment_method, history.payment_method);

        if (RentalHistoryList -> next != NULL)
        {
            rentalHistoryNode -> next = RentalHistoryList -> next;
            RentalHistoryList -> next = rentalHistoryNode;
        }
        else
        {
            RentalHistoryList -> next = rentalHistoryNode;
        }
    }


    fclose(rentalHistoryDB);

    mainMenu();


    return 0;
}

/// Color Creation
void red()
{
    printf("\033[1;31m");
}

/// Color Reset
void reset()
{
    printf("\033[0m");
}

/// show main menu
void mainMenu()
{
    showName("BiCycle Management System");

    if(systemSettingList && systemSettingList -> info.maintenance_mode)
    {
        char ch;
        char maintenance_break[]=".....Maintenance Break.....";
        char maintenance_message[] ="Sorry, the servers are under maintenance.";
        char maintenance_message2[]="Please try again in about 30 minutes.....";
        char success_message[]="The system is now back online and ready for use.";

        printf("\n\t\t\t\t\t");
        for(int i = 0; i < strlen(maintenance_break); i++)
        {
            printf("%c",maintenance_break[i]);
            Sleep(40);
        }
        printf("\n\n\t\t\t\t");
        for(int i = 0; i < strlen(maintenance_message); i++)
        {
            printf("%c",maintenance_message[i]);
            Sleep(40);
        }
        printf("\n\t\t\t\t");
        for(int i = 0; i < strlen(maintenance_message2); i++)
        {
            printf("%c",maintenance_message2[i]);
            Sleep(40);
        }
        printf("\n\t\t\t\t");
        fflush(stdin);
        ch = getchar();
        if(ch == 'Y')
        {
            systemSettingList -> info.maintenance_mode = 0;
            printf("\n\t\t\t\t");
            for(int i = 0; i < strlen(success_message); i++)
            {
                printf("%c",success_message[i]);
                Sleep(40);
            }
            Sleep(2000);
        }
        system("cls");
        mainMenu();
        return;
    }

    printf("\n\t\t\t\t1. Continue as Student\n");
    printf("\t\t\t\t2. Continue as Admin\n");
    printf("\t\t\t\t3. Rules & Regulations\n");
    printf("\t\t\t\t4. About the Developers\n");
    printf("\t\t\t\t0. Exit\n");
    printf("\n\t\t\t\t-----------------------------------------\n\n");
    printf("\t\t\t\tChoose what you want(1-4): ");
    scanf("%d", &choice);

    switch(choice)
    {
    case 0:
    {
        char thanks_message[100]="THANKS FOR TAKING OUR SERVICE............\n";
        printf("\n\t\t\t\t");
        for(int i = 0; i < strlen(thanks_message); i++)
        {
            printf("%c",thanks_message[i]);
            Sleep(50);
        }
        break;
    }
    case 1:
    {
        system("cls");
        studentIntermediateMenu();
        break;
    }

    case 2:
    {
        system("cls");
        adminLogin();
        break;
    }

    case 3:
    {
        system("cls");
        showName("BiCycle Management System");
        rulesRegulations();

        printf("\n\t\t\t\t..........Press enter to choose again..........");
        fflush(stdin);
        getchar();
        system("cls");
        mainMenu();

        break;
    }

    case 4:
    {
        system("cls");
        aboutDevelopers();
        break;
    }

    default:
    {
        printf("\n\n\t\t\t\tInvalid Choice!!!\n");
        printf("\n\t\t\t\tPress enter to choose again....");
        fflush(stdin);
        getchar();
        system("cls");
        mainMenu();
    }

    }

}

/// show menu name
void showName(char *name)
{

    int len=strlen(name), space=(41-len)/2;

    printf("\n\n\t\t\t\t|***************************************|\n");

    red();

    printf("\t\t\t\t|");
    for(int i=1; i<=space-1; i++)
    {
        printf(" ");
    }

    printf("%s", name);
    if(len%2==0) ++space;
    for(int i=1; i<=space; i++)
    {
        if(i==space) printf("|\n");
        else printf(" ");

    }

    reset();

    printf("\t\t\t\t|***************************************|\n");

}

/// initialize system settings
void initSystemSetting(systemSettingNode *systemSettingList)
{
    if(systemSettingList)
    {
        systemSettingList -> info.maintenance_mode = 0;
        systemSettingList -> info.admin_login = 0;
        systemSettingList -> info.student_login = 0;
        strcpy(systemSettingList -> info.loggedin_admin_username, "");
        strcpy(systemSettingList -> info.loggedin_student_username, "");
    }
    else
    {
        printf("\n\t\t\t\tMemory allocation failure\n");
    }
}

/// take a maintenance break
void maintenanceBreak(systemSettingNode *systemSettingList)
{
    if(systemSettingList)
    {
        systemSettingList -> info.maintenance_mode = 1;
        printf("\n\t\t\t\tWe're about to take a break");
        for(int i=0 ; i < 16; i++)
        {
            printf(".");
            Sleep(50);
        }
        Sleep(1500);
        system("cls");
        mainMenu();
    }
    else
    {
        printf("\n\t\t\t\tMemory allocation failure\n");
    }
}

// Hashing the password for security
void hashPassword(char *password)
{

    char *hash_password = password;

    while (*hash_password != '\0')
    {
        if (islower(*hash_password))
        {
            if (*hash_password % 2 == 0)
            {
                *hash_password = (*hash_password + 3);
            }
            else
            {
                *hash_password = (*hash_password - 3);
            }
        }
        else if (isupper(*hash_password))
        {
            if (*hash_password % 2 == 0)
            {
                *hash_password = (*hash_password - 5);
            }
            else
            {
                *hash_password = (*hash_password + 5);
            }
        }
        else if (isdigit(*hash_password))
        {
            if ((*hash_password - '0') % 2 == 0)
            {
                *hash_password = (*hash_password + 2);
            }
            else
            {
                *hash_password = (*hash_password - 2);
            }
        }
        else
        {
            *hash_password = *hash_password;
        }

        hash_password++;
    }
}

/// admin login menu
void adminLogin()
{
    system("cls");

    showName("Continuing as Admin");

    char inputUsername[MAX_USERNAME_LENGTH];
    char inputPassword[MAX_PASSWORD_LENGTH];

    printf("\t\t\t\tEnter username: ");
    scanf("%s", inputUsername);

    printf("\t\t\t\tEnter password: ");
    scanf("%s", inputPassword);

    printf("\n\t\t\t\tAutheticating");
    for(int i=0 ; i < 28; i++)
    {
        printf(".");
        Sleep(30);
    }

    /// Authenticate admin
    if(authenticateAdmin(adminUserList, inputUsername, inputPassword))
    {
        printf("\n\t\t\t\tLogin successful. Welcome, %s!...", inputUsername);
        Sleep(1000);
        system("cls");
        adminMainMenu(inputUsername);
    }
    else
    {
        printf("\n\n\t\t\t\tLogin failed!! Invalid username or password....\n");
        printf("\n\t\t\t\tPress enter to login again....");
        fflush(stdin);
        getchar();
        system("cls");
        adminLogin();
    }
}

/// admin after login options
void adminMainMenu(char *username)
{
    int superadmin = 0;

    if(strcmp(username,"perplexahedron") == 0)
    {
        superadmin = 1;
    }

    char welcome[] = "Welcome, ";
    strcat(welcome, username);

    showName(welcome);
    printf("\n\t\t\t\t1. Add Bicycle\n");
    printf("\t\t\t\t2. Update Bicycle\n");
    printf("\t\t\t\t3. Remove Bicycle\n");
    printf("\t\t\t\t4. View Bicycle Inventory\n");
    printf("\t\t\t\t5. View Rental History\n");
    printf("\t\t\t\t6. User Management\n");
    printf("\t\t\t\t7. View Complains & Review\n");

    if( superadmin )
    {
        printf("\t\t\t\t8. Manage Admin\n");
        printf("\t\t\t\t9. Maintenance Mode\n");
    }
    else
    {
        printf("\t\t\t\t8. Change Password\n");
    }
    printf("\t\t\t\t0. Log Out\n");
    printf("\n\t\t\t\t-----------------------------------------\n\n");
    printf("\t\t\t\tChoose what you want: ");
    scanf("%d", &choice);

    switch(choice)
    {
    case 0:
    {
        fflush(stdin);
        char ch;
        printf("\n\t\t\t\tAre you sure to log out? (y/n): ");
        ch = getchar();

        if(tolower(ch) == 'y')
        {
            system("cls");
            mainMenu();
        }
        else
        {
            printf("\n\t\t\t\tLog out cancelled....");
            Sleep(1500);
            system("cls");
            adminMainMenu(username);
        }
        break;
    }
    case 1:
    {
        system("cls");
        fflush(stdin);

        showName(welcome);
        int inventory_quantity;
        char brand[100], model[100];
        double rental_cost;

        bicycleInfo info;

        info.id = getUniqueBicycleId(bicycleList)+1;

        // Brand Name
        printf("\t\t\t\tEnter Brand Name: ");
        scanf("%s", brand);
        strcpy(info.brand, brand);

        // Model Name
        printf("\t\t\t\tEnter Model Name: ");
        scanf("%s", model);
        strcpy(info.model, model);

        // Rentel Cost
        printf("\t\t\t\tEnter the rent price: ");
        scanf("%lf",&rental_cost);
        info.rental_cost = rental_cost;

        // Inventory Quantity
        printf("\t\t\t\tEnter the inventory quantity: ");
        scanf("%d",&inventory_quantity);
        info.inventory_quantity = inventory_quantity;

        int successUpdate = addBicycle(bicycleList, info, 0);

        if(successUpdate)
        {
            printf("\n\t\t\t\tBicycle Addedd Successfully\n");
        }
        else
        {
            printf("\t\t\t\tFailed to add cycle to the list\n");
        }

        printf("\n\t\t\t\tPress enter to choose again...");
        fflush(stdin);
        getchar();
        system("cls");
        adminMainMenu(username);

        break;
    }
    case 2:
    {
        system("cls");
        showName(welcome);
        int inventory = displayBicycles(bicycleList);

        if(inventory)
        {

            int cycleId;
            printf("\n\t\t\t\tEnter the Bicycle ID: ");
            scanf("%d",&cycleId);

            int updateConfirmation = updateCycle(bicycleList, cycleId, username);

            if(updateConfirmation == 1)
            {
                printf("\n\t\t\t\tBicycle updated Successfully\n");

                FILE *bicycleInfoDBTemp;

                bicycleInfoDB = fopen("bicycle_info.txt", "r");
                bicycleInfoDBTemp = fopen("temp_bicycle_info.txt", "w");

                if (bicycleInfoDB == NULL || bicycleInfoDBTemp == NULL) {
                    printf("Error opening files.\n");
                    return;
                }

                bicycle *current = bicycleList;
                while (current -> next != NULL) {
                    current = current->next;
                    fprintf(bicycleInfoDBTemp, "%d %s %s %.2lf %d\n", current->info.id, current->info.brand, current->info.model, current->info.rental_cost, current ->info.inventory_quantity);
                }

                fclose(bicycleInfoDB);
                fclose(bicycleInfoDBTemp);

                remove("bicycle_info.txt");
                rename("temp_bicycle_info.txt", "bicycle_info.txt");

            }
            else if(updateConfirmation == -1)
            {
                printf("\n\t\t\t\tCycle update info canceled.\n");
            }
            else if(updateConfirmation == -2)
            {
                printf("\n\t\t\t\tInvalid ID....Please check the ID again...\n");
            }
            else
            {
                printf("\t\t\t\tFailed to update cycle to the list\n");
            }

        }

        printf("\n\t\t\t\tPress enter to choose again...");
        fflush(stdin);
        getchar();
        system("cls");
        adminMainMenu(username);
    }
    case 3:
    {
        system("cls");
        showName(welcome);
        int inventory = displayBicycles(bicycleList);

        if(inventory)
        {

            int cycleId;
            printf("\n\t\t\t\tEnter the Bicycle ID: ");
            scanf("%d",&cycleId);

            int removeConfirmation = removeCycle(bicycleList, cycleId);

            if(removeConfirmation == 1)
            {
                printf("\n\t\t\t\tBicycle removed Successfully.\n");

                FILE *bicycleInfoDBTemp;

                bicycleInfoDB = fopen("bicycle_info.txt", "r");
                bicycleInfoDBTemp = fopen("temp_bicycle_info.txt", "w");

                if (bicycleInfoDB == NULL || bicycleInfoDBTemp == NULL) {
                    printf("Error opening files.\n");
                    return;
                }

                bicycle *current = bicycleList;
                while (current -> next != NULL) {
                    current = current->next;
                    fprintf(bicycleInfoDBTemp, "%d %s %s %.2lf %d\n", current->info.id, current->info.brand, current->info.model, current->info.rental_cost, current ->info.inventory_quantity);
                }

                fclose(bicycleInfoDB);
                fclose(bicycleInfoDBTemp);

                remove("bicycle_info.txt");
                rename("temp_bicycle_info.txt", "bicycle_info.txt");

            }
            else if(removeConfirmation == -1)
            {
                printf("\n\t\t\t\tBiCycle removal canceled.\n");
            }
            else if(removeConfirmation == -2)
            {
                printf("\n\t\t\t\tInvalid ID....Please check the ID again...\n");
            }
            else
            {
                printf("\t\t\t\tFailed to remove BiCycle from the list!!\n");
            }

        }

        printf("\n\t\t\t\tPress enter to choose again...");
        fflush(stdin);
        getchar();
        system("cls");
        adminMainMenu(username);

        break;
    }
    case 4:
    {
        system("cls");
        showName(welcome);
        displayBicycles(bicycleList);
        printf("\n\t\t\t\tPress enter to choose again...");
        fflush(stdin);
        getchar();
        system("cls");
        adminMainMenu(username);
        break;
    }
    case 5:
    {
        system("cls");
        showName(welcome);

        viewRentalHistory(RentalHistoryList, "all");

        printf("\n\n\t\t\t\tPress enter to choose again...");
        fflush(stdin);
        getchar();
        system("cls");
        adminMainMenu(username);
        break;
    }
    case 6:
    {
        system("cls");
        showName(welcome);

        viewUser(studentUserList);
        printf("\n\n\t\t\t\tPress enter to choose again...");
        fflush(stdin);
        getchar();
        system("cls");
        adminMainMenu(username);

        break;
    }
    case 7:{
        system("cls");
        showName(welcome);

        char coming_soon[100]="...........FEATURE COMING SOON...........";

        printf("\n\t\t\t\t");

        for(int i = 0; i < strlen(coming_soon); i++){
            printf("%c", coming_soon[i]);
            Sleep(40);
        }

        printf("\n");

        printf("\n\n\t\t\t\tPress enter to choose again...");
        fflush(stdin);
        getchar();
        system("cls");
        adminMainMenu(username);
    }
    case 8:
    {
        fflush(stdin);
        system("cls");
        showName(welcome);
        if(superadmin)
        {
            manageAdmin(adminUserList, username);
        }
        else
        {
            changeAdminPassword(adminUserList, username);
        }
        break;
    }
    case 9:
    {
        fflush(stdin);
        if( superadmin )
        {
            printf("\n\t\t\t\tAre you sure to take a server maintenance break?(y/n): ");
            char ch;
            ch = getchar();

            if(tolower(ch) == 'y')
            {
                maintenanceBreak(systemSettingList);
            }
            else
            {
                printf("\n\t\t\t\tMaintenance break cancelled....");
                Sleep(1500);
                system("cls");
                adminMainMenu(username);
            }
            break;
        }
        else
        {
            printf("\n\t\t\t\tInvalid choice....\n");
            printf("\n\t\t\t\tPress enter to choose again...");
            fflush(stdin);
            getchar();
            system("cls");
            adminMainMenu(username);
        }
        break;
    }
    default:
    {
        printf("\n\t\t\t\tInvalid choice....\n");
        printf("\n\t\t\t\tPress enter to choose again...");
        fflush(stdin);
        getchar();
        system("cls");
        adminMainMenu(username);
    }
    }
}

/// function to authenticate an admin user
int authenticateAdmin(adminUserNode *head, char *username, char *password)
{
    /// Check if the provided username and password match with any admin

    char hash_passowrd[500];
    strcpy(hash_passowrd,password);
    hashPassword(hash_passowrd);

    adminUserNode *current = head -> next;
    while (current != NULL)
    {
        if (strcmp(username, current->username) == 0 && strcmp(hash_passowrd, current->password) == 0)
            return 1;

        current = current->next;
    }

    return 0;
}

/// function to create a new admin user node
adminUserNode* createAdminUserNode(char *username, char *password, int readValidity)
{

    char hash_passowrd[500];
    strcpy(hash_passowrd,password);
    if( readValidity == 0) hashPassword(hash_passowrd);

    adminUserNode *newNode = (adminUserNode*)malloc(sizeof(adminUserNode));
    if (newNode != NULL)
    {
        strcpy(newNode->username, username);
        strcpy(newNode->password, hash_passowrd);
        newNode->next = NULL;
    }
    return newNode;

}

/// function to insert an admin user into the linked list
void insertAdminUser(adminUserNode *head, char *username, char *password,int readValidity)
{

    adminUserNode *newNode = createAdminUserNode(username, password, readValidity);

    if(readValidity == 0){
        adminDB = fopen("admin_info.txt", "a");
        fprintf(adminDB, "%s %s\n", newNode->username, newNode->password);
        fclose(adminDB);
    }

    if (head -> next != NULL)
    {
        newNode -> next = head -> next;
        head -> next = newNode;
    }
    else
    {
        head -> next = newNode;
    }
}

/// Change admin password
void changeAdminPassword(adminUserNode *adminUserList, char *username)
{
    char old_password[MAX_USERNAME_LENGTH];
    char new_password[MAX_USERNAME_LENGTH];
    char confirm_password[MAX_USERNAME_LENGTH];

    fflush(stdin);
    printf("\n\t\t\t\tEnter your old password: ");
    scanf("%s", old_password);

    printf("\n\t\t\t\tEnter your new password: ");
    scanf("%s", new_password);

    do
    {
        printf("\t\t\t\tConfirm your password: ");
        scanf("%s", confirm_password);

        if(strcmp(new_password, confirm_password) != 0)
        {
            printf("\n\t\t\t\tPassword doesn't match\n");
            printf("\t\t\t\tTry again...");
            Sleep(1000);
            printf("\n");
        }
    }
    while(strcmp(new_password, confirm_password) != 0);

    hashPassword(old_password);
    hashPassword(new_password);

    adminUserNode *current = adminUserList;

    while(current -> next)
    {
        current = current -> next;

        if(strcmp(current -> username, username) == 0) break;

    }

    if(strcmp(current -> password, old_password) == 0)
    {
        strcpy(current -> password, new_password);
        printf("\n\t\t\t\tPassword changed successfully");
        for(int i= 0 ; i < 15; i++)
        {
            printf(".");
            Sleep(30);
        }
        fflush(stdin);
        printf("\n\t\t\t\tPlease enter to view menu...");
        getchar();
        system("cls");
        adminMainMenu(username);
    }
    else
    {
        char welcomeMessage[100]="Welcome, ";
        strcat(welcomeMessage,username);

        printf("\n\t\t\t\tOld Password doesn't Match");
        for(int i= 0 ; i < 15; i++)
        {
            printf(".");
            Sleep(30);
        }
        printf("\n\t\t\t\tPress Enter to try again....");
        getchar();
        system("cls");
        showName(welcomeMessage);
        changeAdminPassword(adminUserList, username);
    }

}


/// function to return unique Bicycle ID
int getUniqueBicycleId(bicycle *bicycleList)
{
    bicycle *current = bicycleList;

    srand(time(NULL));

    // Generate a random number between 1 and 1000
    int id = rand() % 1000;

    if(current -> next != NULL)
    {
        int isFound  = 0;
        while(current -> next)
        {
            current = current -> next;
            if(id == current ->info.id)
            {
                isFound = 1;
                break;
            }

        }
        if(isFound)
        {
            id  = id * (rand() % 10) / 3 + 5;
        }
    }

    return id;
}

/// function to add a bicycle to the linked list
int addBicycle(bicycle* head, bicycleInfo info, int readValidity)
{

    if(head == NULL) return 0;


    bicycle *newCycle=  createBicycle(info);
    if(readValidity == 0){
        bicycleInfoDB = fopen("bicycle_info.txt", "a");
        fprintf(bicycleInfoDB, "%d %s %s %.2lf %d\n", info.id, info.brand, info.model, info.rental_cost, info.inventory_quantity);
        fclose(bicycleInfoDB);
    }
    if (head -> next != NULL)
    {
        newCycle -> next = head -> next;
        head -> next = newCycle;
        return 1;
    }
    else
    {
        head -> next = newCycle;
        return 1;
    }

    return 0;
}

/// function to add a new bicycle
bicycle* createBicycle(bicycleInfo info)
{
    bicycle* newCycle = (bicycle*)malloc(sizeof(bicycle));
    if (newCycle != NULL)
    {
        newCycle -> info.inventory_quantity = info.inventory_quantity;
        newCycle->info.id = info.id;
        strcpy(newCycle->info.brand, info.brand);
        strcpy(newCycle->info.model, info.model);
        newCycle->info.rental_cost = info.rental_cost;
        newCycle->next = NULL;
    }

    return newCycle;
}

/// function to update existing bicycle
int updateCycle(bicycle *head, int cycleId,char *name)
{
    bicycle* current = head ;
    if(current  == NULL || current -> next == NULL)
    {
        return 0;
    }

    int isFOund = 0;

    while(current -> next)
    {
        current = current -> next;
        if(current -> info.id == cycleId)
        {
            isFOund = 1;
            break;
        }
    }

    if(isFOund)
    {
        int updateChoice;
        printf("\n\t\t\t\t1. Update Name\n");
        printf("\t\t\t\t2. Update Model\n");
        printf("\t\t\t\t3. Update Cost\n");
        printf("\t\t\t\t4. Updae Inventory\n");
        printf("\t\t\t\t0. Cancel Update\n");
        printf("\n\t\t\t\tEnter your choice(1-4): ");
        scanf("%d", &updateChoice);
        switch(updateChoice)
        {
        case 0:
        {
            return -1;
            break;
        }
        case 1:
        {
            fflush(stdin);
            char brand[105];
            printf("\n\t\t\t\tEnter updated brand name: ");
            scanf("%s", brand);

            strcpy(current -> info.brand, brand);

            return 1;

            break;
        }
        case 2:
        {
            fflush(stdin);
            char model[105];
            printf("\n\t\t\t\tEnter updated model name: ");
            scanf("%s", model);

            strcpy(current -> info.model, model);

            return 1;

            break;
        }
        case 3:
        {
            double rental_cost;
            printf("\n\t\t\t\tEnter updated rental cost: ");
            scanf("%lf",&rental_cost);

            current -> info.rental_cost = rental_cost;

            return 1;

            break;
        }
        case 4:
        {
            int inventory_quantity;
            printf("\n\t\t\t\tEnter updated inventory: ");
            scanf("%d",&inventory_quantity);

            current -> info.inventory_quantity = inventory_quantity;

            return 1;

            break;
        }
        default:
        {
            printf("\n\t\t\t\tInvalid choice....\n");
            return 0;
        }

        }

    }

    return -2;

}

/// function to remove existing bicycle
int removeCycle(bicycle *head, int cycleId)
{
    bicycle* current = head ;
    if(current  == NULL || current -> next == NULL)
    {
        return 0;
    }

    int isFound = 0;

    while(current -> next)
    {
        if(current -> next -> info.id == cycleId)
        {
            isFound = 1;
            break;
        }

        current = current -> next;
    }

    if(isFound)
    {
        fflush(stdin);
        char ch;
        printf("\n\t\t\t\tAre you sure to remove the Cycle? (y/n): ");
        ch = getchar();
        if(tolower(ch)=='y')
        {
            bicycle *freeTemp  = current -> next;
            current -> next = freeTemp -> next;
            free(freeTemp);
            return 1;
        }
        return -1;
    }

    return -2;
}

/// function to display all available bicycles from the list
int displayBicycles(bicycle* head)
{
    bicycle* current = head -> next;
    if(current == NULL)
    {
        printf("\n\t\t\t\t\tInventory is empty!!\n\n");
        return 0;
    }

    printf("\n\t\t\t\t\t\tBicycle Inventory\n\n");
    printf("\t\t\t%-10s%-15s%-15s%-10s%-15s\n\n", "ID", "Brand", "Model", "Cost", "Inventory");

    while (current != NULL)
    {
        printf("\t\t\t%-10d%-15s%-15s%-10.2lf%-15d\n", current -> info.id, current -> info.brand, current -> info.model, current -> info.rental_cost, current -> info.inventory_quantity);
        current = current->next;
    }
    return 1;
}

/// Manage Admin
void manageAdmin(adminUserNode *adminUserList, char *username)
{

    char welcomeMessage[100]="Welcome, ";
    strcat(welcomeMessage, username);

    printf("\n\t\t\t\t1. Add Admin\n");
    printf("\t\t\t\t2. Update Admin\n");
    printf("\t\t\t\t3. Remove Admin\n");
    printf("\t\t\t\t4. View Admin\n");
    printf("\t\t\t\t0. Go to menu\n");

    printf("\n\t\t\t\t-----------------------------------------\n\n");
    printf("\t\t\t\tChoose what you want: ");
    scanf("%d", &choice);

    switch(choice)
    {
    case 1:
    {
        fflush(stdin);
        system("cls");
        showName(welcomeMessage);
        addAdmin(adminUserList, username);
        break;
    }
    case 2:
    {
        system("cls");
        showName(welcomeMessage);
        int availableAdmin = viewAdmin(adminUserList, username);

        if(availableAdmin)
        {
            fflush(stdin);
            char keyname[MAX_USERNAME_LENGTH];
            printf("\n\t\t\t\tEnter the username: ");
            scanf("%s", keyname);

            int updateConfirmation = updateAdmin(adminUserList, keyname, username);

            if(updateConfirmation == 1)
            {
                printf("\n\t\t\t\tAdmin updated Successfully\n");

                FILE *adminDBTemp;

                adminDB = fopen("admin_info.txt", "r");
                adminDBTemp = fopen("temp_admin_info.txt", "w");

                if (adminDB == NULL || adminDBTemp == NULL) {
                    printf("Error opening files.\n");
                    return;
                }

                adminUserNode *current = adminUserList;
                while (current -> next != NULL) {
                    current = current->next;
                    fprintf(adminDBTemp, "%s %s\n", current->username, current->password);
                }

                fclose(adminDB);
                fclose(adminDBTemp);

                remove("admin_info.txt");
                rename("temp_admin_info.txt", "admin_info.txt");

            }
            else if(updateConfirmation == -1)
            {
                printf("\n\t\t\t\tAdmin update info canceled.\n");

            }
            else if(updateConfirmation == -2)
            {
                printf("\n\t\t\t\tInvalid username....Please check the username again...\n");
            }
            else
            {
                printf("\t\t\t\tFailed to update admin to the list\n");
            }

        }

        printf("\n\t\t\t\tPress enter to choose again...");
        fflush(stdin);
        getchar();
        system("cls");
        showName(welcomeMessage);
        manageAdmin(adminUserList, username);
    }
    case 3:
    {
        fflush(stdin);
        system("cls");
        showName(welcomeMessage);
        int availableAdmin = viewAdmin(adminUserList, username);

        if(availableAdmin)
        {

            char keyname[MAX_USERNAME_LENGTH];
            printf("\n\t\t\t\tEnter the username: ");
            scanf("%s", keyname);

            int removeConfirmation = removeAdmin(adminUserList, keyname);

            if(removeConfirmation == 1)
            {
                printf("\n\t\t\t\tAdmin removed Successfully.\n");

                FILE *adminDBTemp;

                adminDB = fopen("admin_info.txt", "r");
                adminDBTemp = fopen("temp_admin_info.txt", "w");

                if (adminDB == NULL || adminDBTemp == NULL) {
                    printf("Error opening files.\n");
                    return;
                }

                adminUserNode *current = adminUserList;
                while (current -> next != NULL) {
                    current = current->next;
                    fprintf(adminDBTemp, "%s %s\n", current->username, current->password);
                }

                fclose(adminDB);
                fclose(adminDBTemp);

                remove("admin_info.txt");
                rename("temp_admin_info.txt", "admin_info.txt");

            }
            else if(removeConfirmation == -1)
            {
                printf("\n\t\t\t\tAdmin removal canceled.\n");
            }
            else if(removeConfirmation == -2)
            {
                printf("\n\t\t\t\tInvalid Username....Please check the useraname again...\n");
            }
            else
            {
                printf("\t\t\t\tFailed to remove Admin from the list!!\n");
            }

        }

        printf("\n\t\t\t\tPress enter to choose again...");
        fflush(stdin);
        getchar();
        system("cls");
        showName(welcomeMessage);
        manageAdmin(adminUserList, username);
    }
    case 4:
    {
        fflush(stdin);
        system("cls");
        showName(welcomeMessage);
        viewAdmin(adminUserList, username);

        printf("\n\t\t\t\tPress enter to choose again...");
        fflush(stdin);
        getchar();
        system("cls");
        showName(welcomeMessage);
        manageAdmin(adminUserList, username);
        break;
    }
    case 0:
    {
        system("cls");
        adminMainMenu(username);
        break;
    }
    default:
    {
        printf("\n\t\t\t\tInvalid choice....\n");
        printf("\n\t\t\t\tPress enter to choose again...");
        fflush(stdin);
        getchar();
        system("cls");
        showName(welcomeMessage);
        manageAdmin(adminUserList, username);
    }
    }

}

/// Add Admin
void addAdmin(adminUserNode *adminUserList, char *username)
{

    char welcomeMessage[100] = "Welcome, ";
    strcat(welcomeMessage, username);

    char inputUsername[MAX_USERNAME_LENGTH];
    char inputPassword[MAX_PASSWORD_LENGTH];
    char confirmPassword[MAX_PASSWORD_LENGTH];

    printf("\t\t\t\tEnter admin username: ");
    scanf("%s", inputUsername);

    while(true)
    {
        int isUnique = 1;
        adminUserNode *uniqueUsername = adminUserList;
        while(uniqueUsername -> next)
        {
            uniqueUsername = uniqueUsername -> next;
            if(strcmp(uniqueUsername -> username, inputUsername) == 0)
            {
                isUnique = 0;
                break;
            }
        }
        if(isUnique)
        {
            break;
        }
        else
        {
            printf("\t\t\t\tUser is already take....\n");
            printf("\n\t\t\t\tEnter a unique username: ");
            scanf("%s", inputUsername);
        }
    }

    printf("\t\t\t\tEnter admin password: ");
    scanf("%s", inputPassword);

    do
    {
        printf("\t\t\t\tConfirm password: ");
        scanf("%s", confirmPassword);

        if(strcmp(inputPassword,confirmPassword) != 0)
        {
            printf("\n\t\t\t\tPassword doesn't match\n");
            printf("\t\t\t\tTry again...");
            Sleep(1000);
            printf("\n");
        }
    }
    while(strcmp(inputPassword,confirmPassword) != 0);

    insertAdminUser(adminUserList, inputUsername, inputPassword,0);

    printf("\n\t\t\t\tAdmin Addedd Successfully.......");
    Sleep(1500);
    system("cls");
    showName(welcomeMessage);
    manageAdmin(adminUserList, username);

}

int viewAdmin(adminUserNode *adminUserList, char *username)
{

    char welcomeMessage[100] = "Welcome, ";
    strcat(welcomeMessage, username);

    adminUserNode* current = adminUserList -> next;
    if(current == NULL)
    {
        printf("\n\t\t\t\t\tCurrently we don't have any admin!!\n\n");
        return 0;
    }

    printf("\n\t\t\t\t\t\tAdmin List\n");
    printf("\n\t\t\t\t%-25s%-20s\n\n", "Username", "Password");

    while (current != NULL)
    {  if(strcmp(current->username, "perplexahedron") != 0){
            printf("\t\t\t\t%-25s%-20s\n", current -> username, "********" );
        }
        current = current -> next;
    }
    return 1;

}

int updateAdmin(adminUserNode *adminUserList, char *keyname, char *username)
{
    char welcomeMessage[100] = "Welcome, ";

    char updateUsername[MAX_USERNAME_LENGTH];
    char updatePassword[MAX_PASSWORD_LENGTH];
    char confirmPassword[MAX_PASSWORD_LENGTH];

    strcat(welcomeMessage, username);

    adminUserNode* current = adminUserList;

    if(current == NULL || current -> next == NULL)
    {
        return 0;
    }

    int isFound = 0;

    while(current -> next)
    {
        current = current -> next;
        if(strcmp(current ->username, keyname) == 0)
        {
            isFound = 1;
            break;
        }
    }

    if(isFound)
    {
        int updateChoice;
        printf("\n\t\t\t\t1. Update Username\n");
        printf("\t\t\t\t2. Update Password\n");
        printf("\t\t\t\t0. Cancel Update\n");
        printf("\n\t\t\t\tEnter your choice(1-2): ");
        scanf("%d", &updateChoice);
        switch(updateChoice)
        {
        case 0:
        {
            return -1;
            break;
        }
        case 1:
        {
            fflush(stdin);

            printf("\n\t\t\t\tEnter updated username: ");
            scanf("%s", updateUsername);

            while(true)
            {
                int isUnique = 1;
                adminUserNode *uniqueUsername = adminUserList;
                while(uniqueUsername -> next)
                {
                    uniqueUsername = uniqueUsername -> next;
                    if(strcmp(uniqueUsername -> username, updateUsername) == 0)
                    {
                        isUnique = 0;
                        break;
                    }
                }
                if(isUnique)
                {
                    break;
                }
                else
                {
                    if(strcmp(keyname, updateUsername) == 0)
                    {
                        printf("\t\t\t\tCurrent username can't be updated username...\n");
                    }
                    else
                    {
                        printf("\t\t\t\tUser is already taken....\n");
                    }
                    printf("\n\t\t\t\tEnter a unique username: ");
                    scanf("%s", updateUsername);
                }
            }

            strcpy(current ->username, updateUsername);

            return 1;

            break;
        }
        case 2:
        {
            fflush(stdin);
            char ch, new_password[MAX_PASSWORD_LENGTH];
            printf("\n\t\t\t\tWould you like to change your password ?(y/n): ");
            scanf("%c", &ch);

            if(tolower(ch) == 'y')
            {
                printf("\t\t\t\tEnter new password: ");
                scanf("%s", updatePassword);

                do
                {
                    printf("\t\t\t\tEnter Confirm password: ");
                    scanf("%s", confirmPassword);

                    if(strcmp(updatePassword,confirmPassword) != 0)
                    {
                        printf("\n\t\t\t\tPassword doesn't match\n");
                        printf("\t\t\t\tTry again...");
                        Sleep(1000);
                        printf("\n");
                    }
                }
                while(strcmp(updatePassword,confirmPassword) != 0);

                strcpy(new_password,updatePassword);
                hashPassword(new_password);

                strcpy(current -> password, new_password);

                return 1;
            }
            else
            {
                printf("\n\t\t\t\tTry to remember your password..\n");
            }

            return 0;

            break;
        }
        default:
        {
            printf("\n\t\t\t\tInvalid choice....\n");
            return 0;
        }

        }
    }

    return -2;




}

/// function to remove existing bicycle
int removeAdmin(adminUserNode *adminUserList, char *keyname)
{
    adminUserNode* current = adminUserList ;
    if(current  == NULL || current -> next == NULL)
    {
        return 0;
    }

    int isFound = 0;

    while(current -> next)
    {
        adminUserNode *temp=current->next;
        if(strcmp(temp->username, keyname) == 0)
        {
            isFound = 1;
            break;
        }

        current = current -> next;
    }

    if(isFound)
    {
        fflush(stdin);
        char ch;
        printf("\n\t\t\t\tAre you sure to remove the Cycle? (y/n): ");
        ch = getchar();
        if(tolower(ch)=='y')
        {
            adminUserNode *freeTemp  = current -> next;
            current -> next = freeTemp -> next;
            free(freeTemp);
            return 1;
        }
        return -1;
    }

    return -2;
}

/// student login/signup menu
void studentIntermediateMenu()
{
    showName("Continuing as Student");
    printf("\n\t\t\t\t1. Log In\n");
    printf("\t\t\t\t2. Sign Up\n");
    printf("\t\t\t\t0. Go back to Main Menu");
    printf("\n\t\t\t\t-----------------------------------------\n\n");
    printf("\t\t\t\tChoose what you want(1-2): ");
    scanf("%d", &choice);

    switch(choice)
    {
    case 0:
    {
        system("cls");
        mainMenu();
        break;
    }

    case 1:
    {
        system("cls");
        studentLogin(studentUserList);
        break;
    }

    case 2:
    {
        system("cls");
        studentSignup(studentUserList);
        break;
    }

    default:
    {
        printf("\n\n\t\t\t\tInvalid Choice!!!\n");
        printf("\n\t\t\t\tPress enter to choose again....");
        fflush(stdin);
        getchar();
        system("cls");
        studentIntermediateMenu();
    }

    }

}

/// student login menu
void studentLogin(studentUserNode *studentUserList)
{
    system("cls");

    showName("Continuing as Student");

    char inputUsername[MAX_USERNAME_LENGTH];
    char inputPassword[MAX_PASSWORD_LENGTH];

    printf("\t\t\t\tEnter username: ");
    scanf("%s", inputUsername);

    printf("\t\t\t\tEnter password: ");
    scanf("%s", inputPassword);

    printf("\n\t\t\t\tAutheticating");
    for(int i=0 ; i < 28; i++)
    {
        printf(".");
        Sleep(30);
    }

    /// Authenticate student

    if(authenticateStudent(studentUserList, inputUsername, inputPassword))
    {
        printf("\n\t\t\t\tLogin successful. Welcome, %s!...", inputUsername);
        Sleep(1000);
        system("cls");
        studentMainMenu(inputUsername);
    }
    else
    {
        printf("\n\n\t\t\t\tLogin failed!! Invalid username or password.\n");
        printf("\n\t\t\t\tPress enter to login again....");
        fflush(stdin);
        getchar();
        system("cls");
        studentLogin(studentUserList);
    }
}

/// sign up for new student
void studentSignup(studentUserNode *studentUserList)
{
    system("cls");
    showName("Continuing as Student");

    /// to store user inputted data
    char inputUsername[MAX_USERNAME_LENGTH];
    char inputPassword[MAX_PASSWORD_LENGTH];
    char confirmPassword[MAX_PASSWORD_LENGTH];

    printf("\t\t\t\tEnter username: ");
    scanf("%s", inputUsername);

    while(true)
    {
        int isUnique = 1;
        studentUserNode *uniqueUsername = studentUserList;
        while(uniqueUsername -> next)
        {
            uniqueUsername = uniqueUsername -> next;
            if(strcmp(uniqueUsername -> username, inputUsername) == 0)
            {
                isUnique = 0;
                break;
            }
        }
        if(isUnique)
        {
            break;
        }
        else
        {
            printf("\t\t\t\tUser is already take....\n");
            printf("\n\t\t\t\tEnter a unique username: ");
            scanf("%s", inputUsername);
        }
    }

    printf("\t\t\t\tEnter password: ");
    scanf("%s", inputPassword);

    do
    {
        printf("\t\t\t\tConfirm password: ");
        scanf("%s", confirmPassword);

        if(strcmp(inputPassword,confirmPassword) != 0)
        {
            printf("\n\t\t\t\tPassword doesn't match\n");
            printf("\t\t\t\tTry again...");
            Sleep(1000);
            printf("\n");
        }
    }
    while(strcmp(inputPassword,confirmPassword) != 0);

    /// animation
    printf("\n\t\t\t\tProcessing");
    for(int i=0 ; i < 30; i++)
    {
        printf(".");
        Sleep(30);
    }


    insertStudentUser(studentUserList, inputUsername, inputPassword, 0);
    printf("\n\t\t\t\tSuccessfully Signed Up!!\n");
    printf("\n\t\t\t\tPress enter to go back...");
    fflush(stdin);
    getchar();
    system("cls");
    studentIntermediateMenu();
}


/// function to create a new student user node
studentUserNode* createStudentUserNode(char *username, char *password, int readValidity)
{
    char hash_passowrd[500];
    strcpy(hash_passowrd,password);
    if( readValidity == 0) hashPassword(hash_passowrd);

    studentUserNode *newNode = (studentUserNode*)malloc(sizeof(studentUserNode));
    if (newNode != NULL)
    {
        strcpy(newNode->username, username);
        strcpy(newNode->password, hash_passowrd);
        newNode->next = NULL;
    }
    return newNode;
}

/// function to insert a student user into the linked list
void insertStudentUser(studentUserNode *head, char *username, char *password, int readValidity)
{
    studentUserNode *newNode = createStudentUserNode(username, password, readValidity);

    if(readValidity == 0){
        studentDB = fopen("student_info.txt", "a");
        fprintf(studentDB, "%s %s\n", newNode->username, newNode->password);
        fclose(studentDB);
    }

    if (head -> next != NULL)
    {
        newNode -> next = head -> next;
        head -> next = newNode;
    }
    else
    {
        head -> next = newNode;
    }
}

/// function to authenticate a student user
int authenticateStudent(studentUserNode *studentUserList, char *username, char *password)
{

    char hash_passowrd[500];
    strcpy(hash_passowrd,password);
    hashPassword(hash_passowrd);

    studentUserNode *current = studentUserList -> next;

    while (current != NULL)
    {
        if (strcmp(username, current->username) == 0 && strcmp(hash_passowrd, current->password) == 0)
            return 1;

        current = current->next;
    }

    return 0;
}

/// student after login options
void studentMainMenu(char *username)
{
    char welcome[] = "Welcome, ";
    strcat(welcome, username);

    showName(welcome);
    printf("\n\t\t\t\t1. Rent A BiCycle\n");
    printf("\t\t\t\t2. View Rental History\n");
    printf("\t\t\t\t3. Have A Complain?\n");
    printf("\t\t\t\t4. Ratings and Reviews\n");
    printf("\t\t\t\t0. Log Out\n");
    printf("\n\t\t\t\t-----------------------------------------\n\n");
    printf("\t\t\t\tChoose what you want(0-2): ");
    scanf("%d", &choice);

    switch(choice)
    {
    case 0:
    {
        fflush(stdin);
        char ch;
        printf("\n\t\t\t\tAre you sure to log out? (y/n): ");
        ch = getchar();

        if(tolower(ch) == 'y')
        {
            system("cls");
            mainMenu();
        }
        else
        {
            printf("\n\t\t\t\tLog out cancelled....");
            Sleep(1500);
            system("cls");
            studentMainMenu(username);
        }
        break;
    }
    case 1:
    {
        system("cls");
        fflush(stdin);
        showName(welcome);

        int inventory = displayBicycles(bicycleList);

        if(inventory)
        {

            int cycleId;
            printf("\n\t\t\t\tEnter the Bicycle ID: ");
            scanf("%d",&cycleId);

            int rentConfirmation = rentCycle(bicycleList, cycleId, username);

            if(rentConfirmation == 1)
            {
                printf("\n\t\t\t\tBicycle rent Successfully\n");
            }
            else if(rentConfirmation == -1)
            {
                printf("\n\t\t\t\tCycle update info canceled.\n");
            }
            else if(rentConfirmation == -2)
            {
                printf("\n\t\t\t\tInvalid ID....Please check the ID again...\n");
            }
            else
            {
                printf("\t\t\t\tFailed to rent cycle to the list\n");
            }

        }

        printf("\n\t\t\t\tPress enter to choose again...");
        fflush(stdin);
        getchar();
        system("cls");
        studentMainMenu(username);
    }
    case 2:{
        system("cls");
        fflush(stdin);
        showName(welcome);

        viewRentalHistory(RentalHistoryList, username);

        printf("\n\n\t\t\t\tPress enter to choose again...");
        fflush(stdin);
        getchar();
        system("cls");
        studentMainMenu(username);
        break;
    }
    case 3:{
        system("cls");
        fflush(stdin);
        showName(welcome);

        char coming_soon[100]="...........FEATURE COMING SOON..........";

        printf("\n\t\t\t\t");

        for(int i = 0; i < strlen(coming_soon); i++){
            printf("%c", coming_soon[i]);
            Sleep(40);
        }

        printf("\n");

        printf("\n\t\t\t\tPress enter to choose again...");
        fflush(stdin);
        getchar();
        system("cls");
        studentMainMenu(username);
        break;
    }
    case 4:{
        system("cls");
        fflush(stdin);
        showName(welcome);

        char coming_soon[100]="...........FEATURE COMING SOON............";

        printf("\n\t\t\t\t");

        for(int i = 0; i < strlen(coming_soon); i++){
            printf("%c", coming_soon[i]);
            Sleep(40);
        }

        printf("\n");

        printf("\n\t\t\t\tPress enter to choose again...");
        fflush(stdin);
        getchar();
        system("cls");
        studentMainMenu(username);
        break;
    }
    default:
    {
        printf("\n\t\t\t\tInvalid choice....\n");
        printf("\n\t\t\t\tPress enter to choose again...");
        fflush(stdin);
        getchar();
        system("cls");
        studentMainMenu(username);
    }
    }
}


int viewUser(studentUserNode *studentUserList, char *username)
{

    char welcomeMessage[100] = "Welcome, ";
    strcat(welcomeMessage, username);

    studentUserNode* current = studentUserList -> next;
    if(current == NULL)
    {
        printf("\n\t\t\t\t\tCurrently we don't have any user!!\n\n");
        return 0;
    }

    printf("\n\t\t\t\t\t\tUser List\n\n");
    printf("\n\t\t\t\t%-25s%-20s\n\n", "Username", "Password");

    while (current != NULL)
    {
        printf("\t\t\t\t%-25s%-20s\n", current -> username, "********" );
        current = current -> next;
    }
    return 1;
}


int rentCycle(bicycle *bicycleList, int cycleId, char *username){
    bicycle *current = bicycleList;
    if( current == NULL || current -> next == NULL){
        return -1;
    }

    int isFound = 0;

    while(current -> next){
        current = current -> next;
        if(current ->info.id ==  cycleId){
            isFound = 1;
            break;
        }
    }

    if(isFound){

        char transactionID[9]; // 8 characters + null terminator

        // Generate a random transaction ID
        generateTransactionID(transactionID, 8);

        int rent_duration;
        do{
            printf("\n\t\t\t\tEnter the Rent Duration(min): ");
            scanf("%d", &rent_duration);
            if(rent_duration < 0){
                printf("\n\t\t\t\tRent Duration Can't be negative\n");
            }
        }while(rent_duration < 0);


        double rental_cost = rent_duration * current ->info.rental_cost;

        printf("\n\t\t\t\tYou need to pay $%.2lf for this rental.\n\t\t\t\tPlease confirm your payment......", rental_cost);
        Sleep(1500);

        system("cls");
        int paymentConfirmation = paymentForRent(current, rental_cost);

        if(paymentConfirmation){

            rentalHistoryDB = fopen("rental_history.txt", "a");


            RentalHistoryNode *rentalHistoryNode = (RentalHistoryNode*) malloc(sizeof(RentalHistoryNode));
            int rental_id = getUniqueRentalId(RentalHistoryList);

            rentalHistoryNode ->history.rental_id = rental_id;
            rentalHistoryNode ->history.bicycle_id = current ->info.id;
            rentalHistoryNode ->history.rental_duration = rent_duration;
            rentalHistoryNode ->history.rental_cost = rental_cost;
            strcpy(rentalHistoryNode ->history.brand, current->info.brand);
            strcpy(rentalHistoryNode ->history.brand, current->info.brand);
            strcpy(rentalHistoryNode ->history.model, current->info.model);
            strcpy(rentalHistoryNode ->history.username, username);

            if(paymentConfirmation == 1){
                strcpy(rentalHistoryNode ->history.payment_method, "Bkash");
            }
            else{
                strcpy(rentalHistoryNode ->history.payment_method, "Nagad");
            }

            strcpy(rentalHistoryNode ->history.transation_id, transactionID);

            if (RentalHistoryList -> next != NULL)
            {
                rentalHistoryNode -> next = RentalHistoryList -> next;
                RentalHistoryList -> next = rentalHistoryNode;
            }
            else
            {
                RentalHistoryList -> next = rentalHistoryNode;
            }

            fprintf(rentalHistoryDB,"%d %d %d %.2f %s %s %s %s %s\n", rentalHistoryNode ->history.rental_id, rentalHistoryNode ->history.bicycle_id, rentalHistoryNode ->history.rental_duration, rentalHistoryNode ->history.rental_cost, rentalHistoryNode ->history.brand, rentalHistoryNode ->history.model, rentalHistoryNode ->history.username, rentalHistoryNode ->history.transation_id, rentalHistoryNode ->history.payment_method);

            fclose(rentalHistoryDB);

            return 1;
        }
    }

    return -2;
}

int paymentForRent(bicycle *current, double rental_cost){

    int payment_choice;
    showName("Confirm Your Payment");
    printf("\n\t\t\t\t1. Bkash\n");
    printf("\n\t\t\t\t2. Nagad\n");
    printf("\n\t\t\t\t3. iCard (Coming Soon)\n");
    printf("\n\t\t\t\t0. Cancelled\n");
    printf("\n\t\t\t\tEnter your choice: ");

    scanf("%d", &payment_choice);

    switch(payment_choice){
        case 0:{
            return 0;
        }
        case 1:{
            system("cls");
            int confirm_payment = pay_with_bkash_or_nagad(payment_choice, rental_cost);
            if(confirm_payment){
                return 1;
            }
            return 0;
            break;
        }
        case 2:{
            system("cls");
            int confirm_payment = pay_with_bkash_or_nagad(payment_choice, rental_cost);
            if(confirm_payment){
                return 2;
            }
            return 0;
            break;
        }
        case 3:{
            char coming_soon[100]="...........FEATURE COMING SOON...........";
            printf("\n\t\t\t\t");
            for(int i = 0; i < strlen(coming_soon); i++)
            {
                printf("%c",coming_soon[i]);
                Sleep(40);
            }
            printf("\n\t\t\t\tPress enter to choose again....");
            fflush(stdin);
            getchar();
            system("cls");
            paymentForRent(current, rental_cost);
            break;
        }
        default:{
            printf("\n\n\t\t\t\tInvalid Choice!!!\n");
            printf("\n\t\t\t\tPress enter to choose again....");
            fflush(stdin);
            getchar();
            system("cls");
            paymentForRent(current, rental_cost);
        }
    }


}

/// payment gateway
int pay_with_bkash_or_nagad(int payment_choice, double rental_cost){
    char show_message[100] ="Pay with ";
    char phone_number[100];
    int PIN;
    double payment;
    if(payment_choice == 1){
        strcat(show_message, "Bkash");
    }
    else{
        strcat(show_message, "Nagad");
    }
    showName(show_message);

    printf("\n\t\t\t\tYou need to pay $%.2lf for this rental.\n\n", rental_cost);

    printf("\n\t\t\t\tEnter your %s number: ", payment_choice == 1 ? "Baksh": "Nagad");
    scanf("%s", &phone_number);

    printf("\n\t\t\t\tEnter your %s PIN: ", payment_choice == 1 ? "Baksh": "Nagad");
    scanf("%d", &PIN);

    printf("\n\t\t\t\tProcessing");
    for(int i = 0; i < 30; i++){
        printf(".");
        Sleep(30);
    }
    printf("\n");
    int trial = 3;
    do{
        printf("\n\t\t\t\tEnter the payment: ");
        scanf("%lf",&payment);

        if(payment != rental_cost){
            printf("\n\t\t\t\tEnter the correct amount to confirm the rent\n\n");
            --trial;
        }


    }while(payment != rental_cost && trial >=0);

    if(trial < 0){
        printf("\n\t\t\t\t Payment failed.............\n");
        printf("\n\t\t\t\tTry again to rent bicycle.....");
        return 0;
    }

    Sleep(1000);

    char success_message[100]="Payment Successful...............";
    printf("\n\t\t\t\t");
    for(int i = 0; i < strlen(success_message); i++)
    {
        printf("%c",success_message[i]);
        Sleep(40);
    }

    return 1;

}


/// function to return unique Bicycle ID
int getUniqueRentalId(RentalHistoryNode *RentalHistoryList)
{
    RentalHistoryNode *current = RentalHistoryList;

    // Generate a random number between 1 and 1000
    int id = rand() % 1000;

    if(current -> next != NULL)
    {
        int isFound  = 0;
        while(current -> next)
        {
            current = current -> next;
            if(id == current ->history.rental_id)
            {
                isFound = 1;
                break;
            }

        }
        if(isFound)
        {
            id  = id * (rand() % 10) / 3 + 5;
        }
    }

    return id;
}

/// Get Transaction ID

// Function to generate a random uppercase letter
char getRandomUppercaseLetter() {
    return 'A' + rand() % 26;
}

// Function to generate a random digit
char getRandomDigit() {
    return '0' + rand() % 10;
}

// Function to generate a random transaction ID
void generateTransactionID(char *transactionID, int length){
    for (int i = 0; i < length; ++i) {
        if (rand() % 2 == 0) {
            transactionID[i] = getRandomUppercaseLetter();
        } else {
            transactionID[i] = getRandomDigit();
        }
    }
    transactionID[length] = '\0'; // Null-terminate the string
}

/// function to view rental history
int viewRentalHistory(RentalHistoryNode *RentalHistoryList, char *username){
    int isFound = 0;
    RentalHistoryNode* current = RentalHistoryList -> next;
    if(current == NULL)
    {
        printf("\n\t\t\t\t\tRental History is empty!!\n\n");
        return 0;
    }

    printf("\n\t\t\t\t\t\tRental History\n\n");
    printf("+-------------+-------------+---------------+------------+---------------+---------------+---------------+---------------------+---------------------+\n");
    printf("| Rental ID   | Bicycle ID  | Rent Duration | Rent Cost  | Brand         | Model         | Username      | Transaction ID      | Payment Method      |\n");
    printf("+-------------+-------------+---------------+------------+---------------+---------------+---------------+---------------------+---------------------+\n");

    while (current != NULL)
    {
        if(strcmp(username, "all") == 0){
            printf("| %-11d | %-11d | %-13d | %-10.2f | %-13s | %-13s | %-13s | %-19s | %-19s |\n", current ->history.rental_id, current ->history.bicycle_id, current ->history.rental_duration, current ->history.rental_cost, current ->history.brand, current ->history.model, current ->history.username, current ->history.transation_id, current ->history.payment_method);
            printf("+-------------+-------------+---------------+------------+---------------+---------------+---------------+---------------------+---------------------+\n");
        }
        else{
            if(strcmp(username, current ->history.username) == 0){
                isFound = 1;
                printf("| %-11d | %-11d | %-13d | %-10.2f | %-13s | %-13s | %-13s | %-19s | %-19s |\n", current ->history.rental_id, current ->history.bicycle_id, current ->history.rental_duration, current ->history.rental_cost, current ->history.brand, current ->history.model, current ->history.username, current ->history.transation_id, current ->history.payment_method);
                printf("+-------------+-------------+---------------+------------+---------------+---------------+---------------+---------------------+---------------------+\n");
            }
        }
        current = current->next;
    }
    if(!isFound){
        printf("\n\t\t\t\tCouldn't fetch any rental history......");
    }
    return 1;
}

/// Rules and Regulations
void rulesRegulations()
{
    // String array representing rules
    char *rulesTitle[] =
    {
        "Secure Credentials",
        "Adhere to Traffic Laws",
        "Timely Bicycle Return",
        "Respect Others",
        "Report Issues Promptly"
    };
    char *rules[] =
    {
        "Keep login information confidential and do not share it with others.",
        "Follow local traffic regulations and wear mandatory safety gear while riding.",
        "Return bicycles within specified time limits to avoid additional charges.",
        "Respect pedestrians and fellow cyclists, and park bicycles in designated areas.",
        "Report any damage, malfunction, or theft promptly through the designated channels."
    };

    // Print the rules
    for (int i = 0; i < 5; ++i)
    {
        red();
        printf("\n\t\t%d. %s\n", i + 1, rulesTitle[i]);
        reset();
        printf("\n\t\t\t%s\n", rules[i]);
    }
}

/// about developers
void aboutDevelopers()
{
    developerInfo info[5]=
    {
        {1, "Mohammad Tasnim Ahmed", "0242310005101524", "64_C"},
        {2, "S. M. Mahbub Rahman", "0242310005101943", "64_C"},
        {3, "Sabbir Sharif", "0242310005101832", "64_C"},
        {4, "Nahidul Islam Forhad", "0242310005101246", "64_C"},
        {5, "Nafisha Kabir", "0242310005101797", "64_C"}
    };
    showName("About the Developers");
    for(int i=0; i<5; i++)
    {
        printf("\n\t\t\t\t%02d.\n\t\t\t\tName : %s\n\t\t\t\tID : %s\n\t\t\t\tSection : %s\n\t\t\t\tDept. of Computer Science & Engineering\n\t\t\t\tDaffodil International University\n",info[i].serial,info[i].developer_name,info[i].developer_id,info[i].developer_section);
    }
    printf("\n\n\t\t\t\tPress enter to go main menu....");
    fflush(stdin);
    getchar();
    system("cls");
    mainMenu();
}
