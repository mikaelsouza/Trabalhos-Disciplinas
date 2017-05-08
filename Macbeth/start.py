# -*- coding: utf-8 -*-
from collections import defaultdict
from random import randrange
from sys import exit


class Controller:
    def __init__(self):
        self.dialogs = Dialogs()
        self.player = Player()
        self.map = Map()

    def pick_item(self, item):
        if self.map.room_l[self.player.room].remove_item(item):
            if self.player.room == 2 or self.player.room == 3 or self.player.room == 4 or self.player.room == 5 or self.player.room == 7 or self.player.room == 8:
                self.map.room_l[self.player.room].set_action()
            self.player.add_item(item)
            print "Peguei {}".format(item)
        else:
            print "Não foi possível pegar o item {}".format(item)

    def drop_item(self, item):
        if self.player.remove_item(item):
            self.map.rool[self.player.room].add_item(item)
            print "Dropei {}".format(item)
        else:
            print "Não foi possível jogar o item {}".format(item)

    def move_player(self, direction):
        self.player.moved = 1
        if self.map.room_l[self.player.room].get_directions()[direction] is not 0:
            self.player.room = self.map.room_l[self.player.room].get_directions()[direction]
        else:
            print "Não foi possível se mover para esta direção"

    def readline(self):
        self.dialogs.show_room_description(self.map.room_l[self.player.room], self.player)
        self.player.moved = 0
        line = raw_input(">> ")
        if(line != ''):
            line = line.split()
        else:
            return
        if line[0] == 'SALA':
            print self.player.room
            return
        if line[0] == 'LARGAR':
            item = line[1]
            self.player.remove_item(item)
            self.map.room_l[self.player.room].add_item(item)
            return
        if line[0] == 'PEGAR':
            item = line[1]
            if item == 'LIVRO':
                self.dialogs.show_book_description()
            self.pick_item(item)
            return
        if line[0] == 'BOLSA':
            self.dialogs.show_player_items(self.player)
            return
        if line[0] == 'NORTE' or line[0] == 'SUL' or line[0] == 'OESTE' or line[0] == 'LESTE':
            self.dialogs.add_score()
            self.move_player(line[0])
            return
        if line[0] == 'OLHAR':
            self.player.moved = 1
            self.dialogs.show_room_description(self.map.room_l[self.player.room], self.player)
            self.map.room_l[self.player.room].show_room_items()
            self.player.moved = 0
            return
        if line[0] == 'PONTOS':
            self.dialogs.print_points(self.player)
            return
        if line[0] == 'FIM':
            self.dialogs.game_over(self.player)
            exit(0)
        print "Comando nao reconhecido :("
        return


class Player:
    def __init__(self):
        self.killed_dragon = False
        self.alive = True
        self.room = 1
        self.moved = 1
        self.items = {"PERNA": 0,
                      "ASA": 0,
                      "OLHO": 0,
                      "DEDO": 0,
                      "LIVRO": 0,
                      "ESPADA": 0,
                      "PELO": 0}

    def add_item(self, item):
        self.items[item] = 1

    def remove_item(self, item):
        if self.items[item] == 1:
            self.items[item] = 0
            return True
        return False

    def change_room(self, room_number):
        self.room = room_number

    def qnt_items(self):
        sum_items = 0
        for key, value in self.items.iteritems():
            if value == 1:
                sum_items += 1
        return sum_items


class Room:
    def __init__(self, room_number):
        self.room_number = room_number
        self.items = defaultdict(int)
        self.times_visited = 0
        self.action = 0
        self.directions = {}

    def get_room_number(self):
        return self.room_number

    def add_item(self, item):
        self.items[item] = 1

    def remove_item(self, item):
        if self.items[item] == 1:
            self.items[item] = 0
            return True
        return False

    def visit(self):
        if self.times_visited == 0:
            self.times_visited += 1

    def times_visited(self):
        return self.times_visited

    def set_action(self):
        self.action += 1

    def get_action(self):
        return self.action

    def add_directions(self, hash):
        self.directions = hash

    def get_directions(self):
        return self.directions

    def show_room_items(self):
        item_list = []
        for key, value in self.items.iteritems():
            if value == 1:
                item_list.append(key)
        if item_list == []:
            print "Não há items nesta sala."
        else:
            print "Você vê:",
            for el in item_list:
                print el + " ",
            print


class Map:
    def __init__(self):
        self.room_l = {}
        # INICIALIZA LISTA DE SALAS
        for x in xrange(10):
            self.room_l[x + 1] = (Room(x + 1))
        # INICIALIZA ITEMS DE CADA SALA

        self.room_l[4].add_item("PERNA")
        self.room_l[8].add_item("ASA")
        self.room_l[10].add_item("PELO")
        self.room_l[2].add_item("OLHO")
        self.room_l[7].add_item("DEDO")
        self.room_l[3].add_item("LIVRO")
        self.room_l[5].add_item("ESPADA")
        # INICIALIZA CAMINHOS POSSIVEIS
        self.room_l[1].add_directions({"NORTE": 3,
                                       "LESTE": 6,
                                       "OESTE": 5,
                                       "SUL": 8})
        self.room_l[2].add_directions({"NORTE": 0,
                                       "LESTE": 3,
                                       "OESTE": 2,
                                       "SUL": 2})
        self.room_l[3].add_directions({"NORTE": 0,
                                       "LESTE": 4,
                                       "OESTE": 2,
                                       "SUL": 1})
        self.room_l[4].add_directions({"NORTE": 0,
                                       "LESTE": 6,
                                       "OESTE": 0,
                                       "SUL": 6})
        self.room_l[5].add_directions({"NORTE": 0,
                                       "LESTE": 1,
                                       "OESTE": 7,
                                       "SUL": 0})
        self.room_l[6].add_directions({"NORTE": 4,
                                       "LESTE": 9,
                                       "OESTE": 1,
                                       "SUL": 8})
        self.room_l[7].add_directions({"NORTE": 2,
                                       "LESTE": 0,
                                       "OESTE": 5,
                                       "SUL": 0})
        self.room_l[8].add_directions({"NORTE": 0,
                                       "LESTE": 6,
                                       "OESTE": 0,
                                       "SUL": 0})
        self.room_l[9].add_directions({"NORTE": 6,
                                       "LESTE": 0,
                                       "OESTE": 10,
                                       "SUL": 0})
        self.room_l[10].add_directions({"NORTE": 0,
                                        "LESTE": 9,
                                        "OESTE": 0,
                                        "SUL": 0})


class Dialogs:

    def __init__(self):
        self.score = 0
        self.visited_rooms = {}
        self.room_dialogs = {}
        for x in xrange(10):
            self.visited_rooms[x + 1] = 0
            self.room_dialogs[x + 1] = ["Sem descrição"]
        self.room_dialogs[1] = ["Já é noite e você finalmente encontra abrigo em uma caverna escura e fria. Após explorá-la por algum tempo, você encontra um caldeirão fumegando. Ao estalo de um trovão, três bruxas surgem diante de você.\nAs bruxas falam em uma só voz:\n\"Mortal, te convocamos, deixe de lado o medo! Apressa-te em tua missão, não sede hesitante. Ache olhos de salamandra e de sapo, um dedo E os jogue aqui nesse caldeirão fumegante!\nE mais: asas da coruja que aqui ousava voar, Perna de lagarto e pelo de gato Angara. Essas coisas precisamos para o feitiço lançar! Rápido, nos traga e mal nenhum te abaterá\nPois agora nos deixe e vá embora. Nada mais temos a dizer no momento A não ser te avisar que lá fora\nUm dragão pode ser o teu tormento!\"", "Já é noite e você finalmente encontra abrigo em uma caverna escura e fria. Após explorá-la por algum tempo, você encontra um caldeirão fumegando. Ao estalo de um trovão, três bruxas surgem diante de você.\nAs bruxas estão diante de você, com olhos arregalados, esperando que lhes dê algo. Os sons da noite parecem lhe avisar que não é seguro neste lugar continuar.", "Já é noite e você finalmente encontra abrigo em uma caverna escura e fria. Após explorá-la por algum tempo, você encontra um caldeirão fumegando. Ao estalo de um trovão, três bruxas surgem diante de você.\nAs bruxas olham para os itens que você trouxe e, incrédulas, iniciam o feitiço:\n\"Pegue a asa da coruja e a perna ao seu lado, Mais o pelo do gato, aquele elegante Angorá.\nE no caldeirão, tudo deve ser jogado;\nE freneticamente girado pra lá e pra cá.\nQuando a cor for de um vermelho ardente, Jogue o olho da salamandra e o dedo do sapo. Tudo borbulha na sopa escarlate e quente. Tudo borbulha no pote encantado;\nPor fim, despeje o caldo no copo imundo E o beba até que limpo fique o fundo!\"\nSem escolha e cercado pelas bruxas, não lhe resta alternativa a não ser beber o líquido asqueroso...\nÀ medida que a neblina vai se dissipando, você acorda lentamente, deitado no chão com uma taça de vinho ao seu lado. Apesar da dor de cabeça, você sorri e percebe que a manhã trouxe um fim, de fato. O fim de um sonho embriagado! ;)"]
        self.room_dialogs[2] = ["Você voltou no tempo... Parece estar em um castelo medieval na Europa no século XIV.\nHá brasões e emblemas nas paredes cheias de mofo. Em um dos emblemas, você vê o que parece ser um animal no meio do fogo. Abaixo uma inscrição diz \"vulcani\" e, mais abaixo, \"Paracelso\". Por um momento, o animal parece estar olhando em seus olhos.", "Você voltou no tempo... Parece estar em um castelo mediaval na Europa no século XIV.\nHá um emblema na parede, rasgado, como se o fogo houvesse arrancado o olho de um animal."]
        self.room_dialogs[3] = ["Você está em uma biblioteca repleta de livros antigos... Há todos os tipos de livros neste lugar desde livros de culinária a livros de rituais macabros. Você se pergunta o que é este lugar... Um livro está jogado ao chão e chama atenção.", "Você está em uma biblioteca repleta de livros antigos... Há todos os tipos de livros neste lugar desde livros de culinária a livros de rituais macabros. Você se pergunta o que é este lugar..."]
        self.room_dialogs[4] = ["Você se encontra em um quarto com uma grande poltrona virada em direção a um pote com uma perna de lagarto... Ao chegar mais perto você nota que a perna ainda se mexe! Será que ainda está viva?", "Você se encontra em um quarto com uma grande poltrona virada em direção a um pote vazio... Bem, pelo menos tem ar dentro..."]
        self.room_dialogs[5] = ["Você entra em um lugar repleto de armamentos medievais, entre eles uma espada chama sua atenção...", "Você entra em um lugar repleto de armamentos medievais..."]
        self.room_dialogs[6] = ["Você está em um corredor com quatro direções..."]
        self.room_dialogs[7] = ["Você se encontra em um lugar onde o grupo Monty Python está encenando o \"Sapo Crocante\". Você olha pra um confeiteiro que logo lhe diz: \"se eu tirar os ossos, eles não vão ser mais crocantes, né?\" Você então vê uma caixa de chocolate onde está escrito \"Sapos Crocantes\". Ela parece conter uma dúzia de sapos inteiros, cuidadosamente cobertos pelo melhor chocolate.", "Você se encontra em um lugar onde o grupo Monty Python está encenando o \"Sapo Crocante\". Você olha pra um confeiteiro que logo lhe diz: \"se eu tirar os ossos, eles não vão ser mais crocantes, né?\" Você então vê uma caixa de chocolate onde está escrito \"Sapos Crocantes\". Ela parece conter uma dúzia de sapos inteiros, cuidadosamente cobertos pelo melhor chocolate."]
        self.room_dialogs[8] = ["Você está em um belo quarto com uma grande cama... Ao olhar as paredes, você nota a imagem de uma coruja em um quadro... Que peculiar! Uma das asas deste coruja é de verdade! Você não sabe se fica impressionado ou assustado...", "Você está em um belo quarto com uma grande cama... Ao olhar as paredes, você nota a imagem de uma coruja sem uma das asas em um quadro... Que peculiar... e esquisito."]
        self.room_dialogs[9] = ["Você está em lugar escuro e úmido... ouve um barulho, como se algo estivesse rastejando. Assustado, você se abaixa tentando entender o que está acontecendo e, de repente, vê um par de olhos na espreita. A última coisa que pensa é como seria bom se tivesse algo para se defender. Antes que possa esboçar qualquer reação, é atacado por um dragão que não lhe dá qualquer chance. Que triste fim para um herói.", "Você está em lugar escuro e úmido... ouve um barulho, como se algo estivesse rastejando. Assustado, você se abaixa tentando entender o que está acontecendo e, de repente, vê um par de olhos na espreita. Antes que o dragão possa desferir seu ataque, você salta pela porta entreaberta, fugindo do recinto.", "Você está em lugar escuro e úmido... ouve um barulho, como se algo estivesse rastejando. Assustado, você se abaixa e empunha sua espada. De repente, olhos vermelhos se abrem revelando um dragão ao seu lado. Antes que ele possa lhe atacar, você desfere um golpe mortal e quase não tem não consegue se esquivar do imenso corpo que desaba no chão.\nAssim que se recupera do susto, você percebe que está de frente para a porta que leva a uma cozinha. Através do corredor, você pode ouvir alguém gritando \"Parem! parem!\" ao mesmo tempo que vários gatos miam inquietos. De repente, um barulho abrupto e o completo silêncio.", "Você está em um lugar escuro e úmido, com o cheiro de sangue impregnado. À sua frente está a porta que leva à cozinha. Perto da porta, você pode ouvir miados distantes."]
        self.room_dialogs[10] = ["Você está em uma cozinha. Você então vê, ao fundo, um cozinheiro colocando uma porção de atum em uma panela. Apesar de não ter certeza, parece haver uma espingarda ao lado do fogão. Há pelos brancos voando por todo lado, enquanto o cozinheiro parece falar sozinho: \"impossível trabalhar com todos esses gatos aqui...\"", "Você está em uma cozinha. Você então vê, ao fundo, um cozinheiro colocando uma porção de atum em uma panela. Apesar de não ter certeza, parece haver uma espingarda ao lado do fogão. Há pelos brancos voando por todo lado, enquanto o cozinheiro parece falar sozinho: \"impossível trabalhar com todos esses gatos aqui...\""]

    def add_score(self):
        self.score += 1

    def show_player_items(self, player):
        self.item_list = []
        for key, value in player.items.iteritems():
            if value == 1:
                self.item_list.append(key)
        if(self.item_list == []):
            print("Não há items na bolsa.")
        else:
            print("Os seguintes items estão na bolsa:")
            for el in self.item_list:
                print el + " ",
            print

    def game_over(self, player):
        print "Sua pontuação foi: {}".format(self.score + player.qnt_items())
        exit(0)

    def show_book_description(self):
        print "É um livro antigo escrito em caracteres que você não reconhece. Mas percebe pelos desenhos ao longo das páginas que é sobre armas usadas para caçar animais. Na capa, você vê o desenho de um cavaleiro cravando sua espada em um dragão."

    def print_points(self, player):
        print "Sua pontuação é: {}".format(self.score + player.qnt_items())

    def show_room_description(self, room, player):
        if player.moved == 0:
            return
        room_number = room.get_room_number()
        items = player.items
        if room_number == 1:
            if room.times_visited == 0:
                print self.room_dialogs[room_number][0]
                room.visit()
                return
            elif items['PERNA'] == 1 and items['OLHO'] == 1 and items['PELO'] == 1 and items['DEDO'] == 1:
                print self.room_dialogs[room_number][2]
                self.game_over(player)
                return
            else:
                print self.room_dialogs[room_number][1]
                return
        elif room_number == 2:
            if room.get_action() == 0:
                print self.room_dialogs[room_number][0]
                return
            else:
                print self.room_dialogs[room_number][1]
                return
        elif room_number == 3:
            if room.get_action() == 0:
                print self.room_dialogs[room_number][0]
                return
            else:
                print self.room_dialogs[room_number][1]
                return
        elif room_number == 4:
            if room.get_action() == 0:
                print self.room_dialogs[room_number][0]
                return
            else:
                print self.room_dialogs[room_number][1]
                return
        elif room_number == 5:
            if room.get_action() == 0:
                print self.room_dialogs[room_number][0]
                return
            else:
                print self.room_dialogs[room_number][1]
                return
        elif room_number == 7:
            if room.get_action() == 0:
                print self.room_dialogs[room_number][0]
                return
            else:
                print self.room_dialogs[room_number][1]
                return
        elif room_number == 8:
            if room.get_action() == 0:
                print self.room_dialogs[room_number][0]
                return
            else:
                print self.room_dialogs[room_number][1]
                return
        elif room_number == 9:
            if items['ESPADA'] == 0:
                if randrange(0, 100, 1) > 30:
                    player.alive = False
                    print self.room_dialogs[room_number][0]
                    self.print_points(player)
                    exit()
                else:
                    print self.room_dialogs[room_number][1]
                    player.change_room(6)
            elif player.killed_dragon is False:
                print self.room_dialogs[room_number][2]
                player.killed_dragon = True
            else:
                print self.room_dialogs[room_number][3]
        else:
            print self.room_dialogs[room_number][0]


def main():
    c = Controller()
    while(True):
        c.readline()

if __name__ == '__main__':
    main()
