#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Created by Rafael S. Guimaraes and Diego Cardoso
# NERDS - Nucleo de Estudos em Redes Definidas por Software
#
import threading
import csv
import netaddr
import time
import curses
import sys
import os

from dpcontrol import *
from lib.bloomcl.entity.datapath import *


class BCLApp(object):

    INIT = {'type': 'init'}

    def __init__(self, menu_options, dp_control):
        self.dp_control = dp_control
        self.screen = curses.initscr()
        self.menu_options = menu_options
        self.selected_option = 0
        self._previously_selected_option = None
        self.running = True
        # init curses and curses input
        # curses.noecho()
        curses.cbreak()
        curses.start_color()
        # Hide cursor
        curses.curs_set(1)
        # set up color pair for highlighted option
        curses.init_pair(1, curses.COLOR_BLACK, curses.COLOR_BLUE)
        curses.init_pair(2, curses.COLOR_YELLOW, curses.COLOR_BLACK)
        curses.init_pair(3, curses.COLOR_BLACK, curses.COLOR_RED)
        curses.init_pair(4, curses.COLOR_BLACK, curses.COLOR_GREEN)
        curses.init_pair(5, curses.COLOR_WHITE, curses.COLOR_BLACK)
        self.screen.keypad(1)

        self.hilite_color = curses.color_pair(1)
        self.normal_color = curses.A_NORMAL

    def prompt_selection(self, parent=None):
        if parent is None:
            lastoption = "Exit"
        else:
            lastoption = "Return to previous menu ({})".format(parent['title'])

        option_count = len(self.menu_options['options'])

        input_key = None

        ENTER_KEY = ord('\n')
        while input_key != ENTER_KEY:
            if self.selected_option != self._previously_selected_option:
                self._previously_selected_option = self.selected_option

            self.screen.border(0)
            self._draw_title()
            for option in range(option_count):
                if self.selected_option == option:
                    self._draw_option(option, self.hilite_color)
                else:
                    self._draw_option(option, self.normal_color)

            if self.selected_option == option_count:
                self.screen.addstr(5 + option_count,
                                   4,
                                   "{:2} - {}".format(option_count+1,
                                                      lastoption),
                                   self.hilite_color)
            else:
                self.screen.addstr(5 + option_count,
                                   4,
                                   "{:2} - {}".format(option_count+1,
                                                      lastoption),
                                   self.normal_color)

            max_y, max_x = self.screen.getmaxyx()
            if input_key is not None:
                self.screen.addstr(max_y-3,
                                   max_x - 5,
                                   "{:3}".format(self.selected_option))
            self.screen.refresh()
            input_key = self.screen.getch()
            down_keys = [curses.KEY_DOWN, ord('j')]
            up_keys = [curses.KEY_UP, ord('k')]
            exit_keys = [ord('q')]

            if input_key in down_keys:
                if self.selected_option < option_count:
                    self.selected_option += 1
                else:
                    self.selected_option = 0

            if input_key in up_keys:
                if self.selected_option > 0:
                    self.selected_option -= 1
                else:
                    self.selected_option = option_count

            if input_key in exit_keys:
                self.selected_option = option_count
                break

        return self.selected_option

    def _draw_option(self, option_number, style):
        self.screen.addstr(5 + option_number,
                           4,
                           "{:2} - {}"
                           .format(
                               option_number+1,
                               self.menu_options['options'][option_number]['title']),
                           style)

    def _draw_title(self):
        self.screen.addstr(2, 2, self.menu_options['title'], curses.A_STANDOUT)
        self.screen.addstr(4, 2, self.menu_options['subtitle'], curses.A_BOLD)

    def display(self):
        selected_option = self.prompt_selection()
        i, _ = self.screen.getmaxyx()
        curses.endwin()
        os.system('clear')
        if selected_option < len(self.menu_options['options']):
            selected_opt = self.menu_options['options'][selected_option]
            return selected_opt
        else:
            self.running = False
            return {'title': 'Exit', 'type': 'exitmenu'}

    def show_result(self, command):
        if command == 1:
            self.screen.addstr(13, 5, "===== BLOOMFILTER TABLE =====")
            for i in range(32):
                try:
                    self.screen.addstr(
                        int(14+i),
                        5,
                        "{} => {}".format(
                            str(self.dp_control._datapath._tablesbf._table[i].bitarray[:35]),
                            str(i + 1)
                            )
                    )
                except Exception as ex:
                    pass
        elif command == 2:
            self.screen.addstr(13, 5, "===== ROUTE TABLE =====")
        elif command == 3:
            if self.dp_control.is_finished_bf:
                self.screen.clear()
                self.screen.border(0)
                self.screen.addstr(2, 2, "IP ADDRESS: ", curses.color_pair(2))
                self.screen.refresh()
                ip = self.screen.getstr(2, 14, 60)
                self.screen.addstr(13, 5, "===== BLOOMFILTER TABLES {} =====".format(ip))
                lst_bf = self.dp_control._datapath._tablesbf.find_ipaddress(ip)
                self.screen.addstr(14, 5, "{}".format(lst_bf))
            else:
                self.screen.addstr(13, 5, "BLOOMFILTER TABLES NOT FINISHED")
        elif command == 4:
            if self.dp_control.is_finished_rt:
                self.screen.clear()
                self.screen.border(0)
                self.screen.addstr(2, 2, "IP ADDRESS: ", curses.color_pair(2))
                self.screen.refresh()
                ip = self.screen.getstr(2, 14, 60)
                self.screen.addstr(13, 5, "===== ROUTE TABLE ID {} ===== ".format(ip))
                index_rt = self.dp_control.find_item_rt(ip)
                self.screen.addstr(14, 5, "INDEX: {}         ".format(index_rt))
            else:
                self.screen.addstr(13, 5, "ROUTE TABLES NOT FINISHED YET")
        elif command == 5:
            if self.dp_control.is_finished_rt:
                self.screen.clear()
                self.screen.border(0)
                self.screen.addstr(2, 2, "IP ADDRESS: ", curses.color_pair(2))
                self.screen.refresh()
                ip = self.screen.getstr(2, 14, 60)
                self.screen.addstr(13, 5, "===== ROUTE TABLE ID {} ===== ".format(ip))
                index_rt = self.dp_control.find_item_rt_seq(ip)
                self.screen.addstr(14, 5, "INDEX: {}         ".format(index_rt))
            else:
                self.screen.addstr(13, 5, "ROUTE TABLES NOT FINISHED YET")


class Main(object):
    """
        Main Class
    """
    def __init__(self):
        self.dp_control = DPControl()
        self.menu = None
        self._t_window = threading.Thread(
                    name="show interface",
                    target=self.window)
        self._t_window.start()

    def window(self):
        menu = {'title': '=========== BloomCL - NERDS ===========',
                'type': 'menu',
                'subtitle': 'Options'}

        option_1 = {'title': 'Show BloomFilter Table',
                    'type': 'command',
                    'command': 1}

        option_2 = {'title': 'Show Route Table',
                    'type': 'command',
                    'command': 2}

        option_3 = {'title': 'Find IP in BloomFilter Table',
                    'type': 'command',
                    'command': 3}

        option_4 = {'title': 'Find IP in Route Table - Parallel',
                    'type': 'command',
                    'command': 4}

        option_5 = {'title': 'Find IP in Route Table - Sequential',
                    'type': 'command',
                    'command': 5}

        menu['options'] = [option_1, option_2, option_3, option_4, option_5]

        self.menu = BCLApp(menu, self.dp_control)
        while True:
            selected_action = self.menu.display()
            if selected_action['type'] != 'exitmenu':
                self.menu.show_result(selected_action['command'])
            else:
                break


def main():
    dp_main = Main()


if __name__ == '__main__':
    main()
