# Write your code here :-)
#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Mon Sep 30 19:31:16 2063

@author: Who Knows!

INVESTMENT CALCULATOR!
"""
import math

print("This program calculates and summarizes the estimated profits from an investment.")

initial_investment    = float(input("Initial investment sum (eur):\n"))
annual_profit_percent = float(input("Expected annual growth / return rate (including expenses) (%):\n"))
per_month_investment  = float(input("Monthly investment (+) or withdrawal (-) (eur):\n"))
years                 = float(input("For how many years are you planning to hold the investment?\n"))

e = "eur"
print('{:>5s} | {:>5s} | {:^10s} | {:^10s} | {:^10s} | {:^10s}'.format('Year','Month','Start','Monthly', 'End', 'Cumulative'))
print('{:>5s} | {:>5s} | {:^10s} | {:^10s} | {:^10s} | {:^10s}'.format('','','balance','Profit', 'balance','Profit'))
print('{:>5s} | {:>5s} | {:>10s} | {:>10s} | {:>10s} | {:>10s}'.format('','',e,e,e,e))
print('-' * 65)

MPP = (1 + (annual_profit_percent / 100)) ** (1 / 12)
RM = years * 12
rM = math.floor(RM)
II = initial_investment
M = 0
Y = 1
CP = 0
EB = 1

for i in range(rM):
    MP = (II * MPP) - II
    EB = (II * MPP) + per_month_investment
    if EB < 0:
        print()
        print("Stopped printing as balance cannot go negative.\n")
        break
    CP = CP + MP
    if M < 12:
        M = M + 1
    else:
        M = 1
        Y = Y + 1
        print('-' * 65)
    print('{:>5.0f} | {:>5.0f} | {:>10.2f} | {:>10.2f} | {:>10.2f} | {:>10.2f}'.format( Y, M, II, MP, EB, CP))
    II = EB
print()
print('End balance:{:>18.2f} eur'.format(EB))
print('Total profit:{:>17.2f} eur'.format(CP))
print('Total net deposit:{:>12.2f} eur'.format(initial_investment + (per_month_investment * rM)))