#include<mysql/mysql.h>
#include<wiringPi.h>
#include<softPwm.h>
#include <stdio.h>
#include <stdlib.h>
unsigned char led[3]={11,13,15};//BGR
unsigned char button[4]={16,18,22,36};
int tt1,tt2,tt3;
int sql1;
int red,green,blue;
int red1,green1,blue1;
int red2,green2,blue2;
int ledState;
int changeState;
int red_web,green_web,blue_web;
MYSQL *conn;
MYSQL_RES *res;
MYSQL_ROW row;

unsigned char control_led(unsigned char red,unsigned char green,unsigned char blue)
{
    softPwmWrite(led[0],blue);
    softPwmWrite(led[1],green);
    softPwmWrite(led[2],red);
    blue1=blue;
    green1=green;
    red1=red;
}
void button1()
{
    sql1=1;
    tt1=tt1+1;
    if(tt1==2) tt1=0;
}
void check_button1()
{
    if(tt1==0)
    {
        control_led(0,0,0);
        ledState=0;
    }
    else if(tt1==1)
    {
        control_led(255,0,0);
        ledState=1;
    }
}
void button2()
{
    sql1=1;
    tt2=tt2+1;
    if(tt2==10) tt2=0;
}
void check_button2()
{
    if(tt1==1){
    if(tt2==0)
    {
        control_led(255,0,0);
    }
    else if(tt2==1)
    {
        control_led(0,255,0);
    }
    else if(tt2==2)
    {
        control_led(0,0,255);
    }
    else if(tt2==3)
    {
        control_led(255,255,0);
    }
    else if(tt2==4)
    {
        control_led(0,255,255);
    }
    else if(tt2==5)
    {
        control_led(255,0,255);
    }
    else if(tt2==6)
    {
        control_led(255,255,255);
    }
    else if(tt2==7)
    {
        control_led(128,128,0);
    }
    else if(tt2==8)
    {
        control_led(128,0,128);
    }
    else if(tt2==9)
    {
        control_led(0,128,128);
    }
    }
}
void button3()
{
    sql1=1;
    changeState=1;
    tt3=tt3+1;
    if(tt3==10) tt3=0;
}
void check_button3()
{   
    blue2=blue1-tt3*30;
    green2=green1-tt3*30;
    red2=red1-tt3*30;
    if(blue2<0) blue2=0;
    if(green2<0) green2=0;
    if(red2<0) red2=0;
    control_led(red2,green2,blue2);
}
void check()
{
    printf("RGB: %d,%d,%d\n",red1,green1,blue1);
}
int main(void)
{   
    char *server = "localhost";
    char *user = "admin";
    char *password ="123456";
    char *database = "smart_room_fn";
    char sql[200];
    //setup library for wiringPi
    wiringPiSetupPhys();
    //Khai bao IO, interupt,softPWM
    for (int i=0;i<3;i++)
    {
        softPwmCreate(led[i],0,255);
    }
    wiringPiISR(button[0],INT_EDGE_RISING,&button1);
    wiringPiISR(button[1],INT_EDGE_RISING,&button2);
    wiringPiISR(button[2],INT_EDGE_RISING,&button3);
    tt1=0;
    tt2=0;
    red1=0;green1=0;blue1=0;ledState=0;tt3=0;
    changeState=0;
    while(1)
    {
        check_button1();
        check_button2();
        check_button3();
        conn = mysql_init(NULL);
        mysql_real_connect(conn,server,user,password,database,0,NULL,0);
        if(sql1==1)
        {
            sprintf(sql,"insert into led_control (red,green,blue,ledState,brightness,changeState) values(%d,%d,%d,%d,%d,%d);",red1,green1,blue1,ledState,tt3,0);
            mysql_query(conn,sql);
            printf("database update\n");
            sql1=0;
        }
        else
        {
            sprintf(sql,"select * from led_control where id>((select max(id) from led_control)-1);");
            mysql_query(conn,sql);
            res = mysql_store_result(conn);
            row = mysql_fetch_row(res);
            if(atoi(row[4])==1)
            {
                control_led(atoi(row[1]),atoi(row[2]),atoi(row[3]));
                if((atoi(row[5])!=0)&&(atoi(row[7])==1))
                {
                    red_web=atoi(row[1])-atoi(row[5])*30;
                    green_web=atoi(row[2])-atoi(row[5])*30;
                    blue_web=atoi(row[3])-atoi(row[5])*30;
                    if(red_web<0) red_web=0;
                    if(blue_web<0) blue_web=0;
                    if(green_web<0) green_web=0;
                    control_led(red_web,green_web,blue_web);
                // sprintf(sql,"update led_control set changeState=0 where id>((select max(id) from led_control)-1);");
                // mysql_query(conn,sql);
                }

            }
            else
            {
                control_led(0,0,0);
            }
        }
        check();
        printf("rgb values are updated\n");
        mysql_close(conn);
        delay(200);
    }
    return 0;
}