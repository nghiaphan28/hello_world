#include<mysql/mysql.h>
#include<wiringPi.h>
#include<softPwm.h>
#include <stdio.h>
#include <stdlib.h>
#define IN1 37
#define IN2 31
#define ENA 29
#define PWM_INIT 10
unsigned char button[4]={16,18,22,36};
int tt4_pi,fan_level_pi;
int state_web,fan_web;
int sql_fan;
MYSQL *conn;
MYSQL_RES *res;
MYSQL_ROW row;
char sql[200];
// void button4()
// {
//     sql_fan=1;
//     tt4_pi=tt4_pi+1;
// }
void check_fan()
{   
    if(tt4_pi==0)
    {
        digitalWrite(IN1,LOW);
        digitalWrite(IN2,LOW);
        softPwmWrite(ENA,0);
        printf("MOTOR OFF!\n");
        fan_level_pi=0;
    }
    else if(tt4_pi==1)
    {
        digitalWrite(IN1,HIGH);
        digitalWrite(IN2,LOW);
        softPwmWrite(ENA,PWM_INIT);
        printf("MOTOR ON!\n");
        fan_level_pi=1;   
    }
    else if(tt4_pi==2) tt4_pi=0; 
}
void check_button()
{
    if(digitalRead(button[3])==1)
    {
        sql_fan=1;
        tt4_pi=tt4_pi+1;
    }
}
int main (void)
{
    //connect to database
    char *server = "localhost";
    char *user = "admin";
    char *password ="123456";
    char *database = "smart_room_fn";
    //set up library 
    wiringPiSetupPhys();
    //set up GPIO
    pinMode(IN1,OUTPUT);
    pinMode(IN2,OUTPUT);
    pinMode(button[3],INPUT);
    softPwmCreate(ENA,0,20);
    // wiringPiISR(button[3],INT_EDGE_RISING,&button4);   
    //Ready- state
    digitalWrite(IN1,LOW);
    digitalWrite(IN2,LOW);
    // softPwmWrite(ENA,0);
    tt4_pi=0;
    sql_fan=0;
    while(1)
    {
        check_button();
        check_fan();
        conn = mysql_init(NULL);
        mysql_real_connect(conn,server,user,password,database,0,NULL,0);
        if(sql_fan==1)
        {
            sprintf(sql,"insert into fan_control (fanState,fanLevel) values (%d,%d);",tt4_pi,fan_level_pi);
            mysql_query(conn,sql);
            printf("Fan database updated from Pi!\n");
            sql_fan=0;
        }
        else
        {
            sprintf(sql,"select * from fan_control where id>(select max(id) from fan_control)-1");
            mysql_query(conn,sql);
            res = mysql_store_result(conn);
            row = mysql_fetch_row(res);
            if(atoi(row[1])==0)
            {
                digitalWrite(IN1,LOW);
                digitalWrite(IN2,LOW);
                softPwmWrite(ENA,0);
            }
            else
            {
                // if(atoi(row[2])==0)
                // {
                //     digitalWrite(IN1,LOW);
                //     digitalWrite(IN2,LOW);
                //     softPwmWrite(ENA,0);
                // }
                // else
                // {
                //     digitalWrite(IN1,HIGH);
                //     digitalWrite(IN2,LOW);
                //     softPwmWrite(ENA,atoi(row[2])+5);
                // }
                digitalWrite(IN1,HIGH);
                digitalWrite(IN2,LOW);
                softPwmWrite(ENA,PWM_INIT);
            }

        }
        printf("Fan updated\n");
        mysql_close(conn);
        delay(200);


    }
    return 0;
}