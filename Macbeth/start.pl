:- use_module(library(random)).
:- dynamic at_second_visit/1, at_item_obtained/1, at/2, alive/1, i_am_at/1, chance/1.

/* Onde nosso personagem está */

i_am_at(q_1).

/* Lógica do mapa do jogo */

path(q_1, norte, q_3).
path(q_1, sul, q_8).
path(q_1, leste, q_6).
path(q_1, oeste, q_5).

path(q_2, sul, q_2).
path(q_2, leste, q_3).
path(q_2, oeste, q_2).

path(q_3, sul, q_1).
path(q_3, leste, q_4).
path(q_3, oeste, q_2).

path(q_4, sul, q_6).
path(q_4, leste, q_6).
path(q_4, oeste, q_3).

path(q_5, leste, q_1).
path(q_5, oeste, q_7).

path(q_6, norte, q_4).
path(q_6, sul, q_8).
path(q_6, leste, q_9).
path(q_6, oeste, q_1).

path(q_7, norte, q_2).
path(q_7, oeste, q_5).

path(q_8, leste, q_6).
path(q_8, oeste, q_1).

path(q_9, norte, q_6).
path(q_9, oeste, q_10).

path(q_10, leste, q_9).

/* Onde os items estão */

at(dedo, q_7).
at(asa, q_8).
at(olho, q_2).
at(perna, q_4).
at(pelo, q_10).
at(livro, q_3).
at(espada, q_5).

/* Fatos que especificam que o dragão está vivo */

alive(dragon).

/* Regra para saber em que sala estou */

sala :-
	i_am_at(Place),
	write(Place),
	nl, !.

/* Regras para descrever como pegar objetos e largar objetos */

pegar(X) :-
	at(X, in_pocket),
	write('Você já está com este item.'),
	nl, !.

pegar(livro) :-
	i_am_at(Place),
    at(livro, Place),
    assert(at_item_obtained(Place)),
    retract(at(livro, Place)),
    assert(at(livro, in_pocket)),
    write('É um livro antigo escrito em caracteres que você não reconhece. Mas percebe pelos desenhos ao longo das páginas que é sobre armas usadas para caçar animais. Na capa, você vê o desenho de um cavaleiro cravando sua espada em um dragão.'), nl,
    write('OK.'),
    nl, !.

pegar(X) :-
    i_am_at(Place),
    at(X, Place),
    assert(at_item_obtained(Place)),
    retract(at(X, Place)),
    assert(at(X, in_pocket)),
    write('OK.'),
    nl, !.

pegar(_) :-
    write('Não foi possível pegar o objeto'),
    nl.

largar(X) :-
    at(X, in_pocket),
    i_am_at(Place),
    retract(at(X, in_pocket)),
    assert(at(X, Place)),
    write('OK.'),
    nl, !.

largar(_) :-
    write('Você não está com o objeto.'),
    nl.

/* Regras para descrever as 4 direções possíveis de ir */

norte :- go(norte).

sul :- go(sul).

leste :- go(leste).

oeste :- go(oeste).

/* Está regra mostra como se mover para uma direção */

go(Direction) :-
	i_am_at(Here),
	path(Here, Direction, There),
	retract(i_am_at(Here)),
	assert(i_am_at(There)),
	look, !.

go(Direction) :-
	i_am_at(Here),
	path(Here, Direction, There),
	retract(i_am_at(Here)),
	assert(i_am_at(There)),
	look, !.

go(_) :-
	write('Não é possível ir por esta direção.').

/* Esta regra mostra como olhar a sala */

olhar :- look.

look :-
	i_am_at(Place),
	describe(Place),
	assert(at_second_visit(Place)),
	nl,
	notice_objects_at(Place),
	!, nl.

look :-
	at_second_visit(Place),
	i_am_at(Place),
	describe(Place),
	nl,
	notice_objects_at(Place),
	nl.

/* Estas regras servem para mostrar os objetos dentro da sala */

notice_objects_at(Place) :-
	at(X, Place),
	write('Tem um '), write(X), write(' aqui.'), nl,
	fail.

notice_objects_at(_).

/* Regra para olhar items que estão na bolsa */

notice_my_objects :-
	at(X, in_pocket),
	write('Items na minha bolsa: '), write(X), nl, fail.

notice_my_objects :-
	write('Minha bolsa está vazia.'), nl, !.

bolsa :- notice_my_objects.

/* Esta regra mostra os pontos do jogador */

pontos :- points.

points :-
	at(_, in_pocket),
	write('Seu score de salas é: '), nl,
	aggregate_all(count, at_second_visit(_), Room_score),
	write('Room_score = '), write(Room_score), nl,
	write('Seu score de items é: '), nl,
	aggregate_all(count, at(_, in_pocket), Item_score),
	write('Item_score = '), write(Item_score),
	!.

points :-
	write('Seu score de salas é: '), nl,
	aggregate_all(count, at_second_visit(_), Room_score),
	write('Room_score = '), write(Room_score), nl,
	write('Seu score de items é: '), nl,
	write('Items_Score = 0'),
	!.


/* Esta regra mostra o estado de morte do jogador */

die :-
	!, finish.

fim :- finish.

finish :-
	at(_, in_pocket),
	write('Seu score de salas é: '), nl,
	aggregate_all(count, at_second_visit(_), Room_score),
	write('Room_score = '), write(Room_score), nl,
	write('Seu score de items é: '), nl,
	aggregate_all(count, at(_, in_pocket), Item_score),
	write('Item_score = '), write(Item_score), nl,
	halt,
	!.

finish :-
	write('Seu score de salas é: '), nl,
	aggregate_all(count, at_second_visit(_), Room_score),
	write('Room_score = '), write(Room_score), nl,
	write('Seu score de items é: '), nl,
	write('Items_Score = 0'), nl,
	halt,
	!.

/* Esta regra deve ser executada no inicio do jogo */

start :-
	look.

/* Estas regras descrevem varias salas dependendo das circunstâncias */

describe(q_1) :-
	at(dedo, in_pocket),
	at(pelo, in_pocket),
	at(olho, in_pocket),
	at(perna, in_pocket),
	write('Já é noite e você finalmente encontra abrigo em uma caverna escura e fria. Após explorá-la por algum tempo, você encontra um caldeirão fumegando. Ao estalo de um trovão, três bruxas surgem diante de você.\nAs bruxas olham para os itens que você trouxe e, incrédulas, iniciam o feitiço:\n\"Pegue a asa da coruja e a perna ao seu lado, Mais o pelo do gato, aquele elegante Angorá.\nE no caldeirão, tudo deve ser jogado;\nE freneticamente girado pra lá e pra cá.\nQuando a cor for de um vermelho ardente, Jogue o olho da salamandra e o dedo do sapo. Tudo borbulha na sopa escarlate e quente. Tudo borbulha no pote encantado;\nPor fim, despeje o caldo no copo imundo E o beba até que limpo fique o fundo!\"\nSem escolha e cercado pelas bruxas, não lhe resta alternativa a não ser beber o líquido asqueroso...\nÀ medida que a neblina vai se dissipando, você acorda lentamente, deitado no chão com uma taça de vinho ao seu lado. Apesar da dor de cabeça, você sorri e percebe que a manhã trouxe um fim, de fato. O fim de um sonho embriagado! ;)'),
	nl,
	finish,
	!.

describe(q_1) :-
	at_second_visit(q_1),
	write('Já é noite e você finalmente encontra abrigo em uma caverna escura e fria. Após explorá-la por algum tempo, você encontra um caldeirão fumegando. Ao estalo de um trovão, três bruxas surgem diante de você.\nAs bruxas estão diante de você, com olhos arregalados, esperando que lhes dê algo. Os sons da noite parecem lhe avisar que não é seguro neste lugar continuar.'),
	nl,
	!.

describe(q_1) :-
	write('Já é noite e você finalmente encontra abrigo em uma caverna escura e fria. Após explorá-la por algum tempo, você encontra um caldeirão fumegando. Ao estalo de um trovão, três bruxas surgem diante de você.\nAs bruxas falam em uma só voz:\n\"Mortal, te convocamos, deixe de lado o medo! Apressa-te em tua missão, não sede hesitante. Ache olhos de salamandra e de sapo, um dedo E os jogue aqui nesse caldeirão fumegante!\nE mais: asas da coruja que aqui ousava voar, Perna de lagarto e pelo de gato Angara. Essas coisas precisamos para o feitiço lançar! Rápido, nos traga e mal nenhum te abaterá\nPois agora nos deixe e vá embora. Nada mais temos a dizer no momento A não ser te avisar que lá fora\nUm dragão pode ser o teu tormento!\"'),
	nl,
	!.

describe(q_2) :-
	at_item_obtained(q_2),
	write('Você voltou no tempo... Parece estar em um castelo mediaval na Europa no século XIV.\nHá um emblema na parede, rasgado, como se o fogo houvesse arrancado o olho de um animal.'),
	nl,
	!.

describe(q_2) :-
	write('Você voltou no tempo... Parece estar em um castelo medieval na Europa no século XIV.\nHá brasões e emblemas nas paredes cheias de mofo. Em um dos emblemas, você vê o que parece ser um animal no meio do fogo. Abaixo uma inscrição diz \"vulcani\" e, mais abaixo, \"Paracelso\". Por um momento, o animal parece estar olhando em seus olhos.'),
	nl,
	!.

describe(q_3) :-
	at_item_obtained(q_3),
	write('Você está em uma biblioteca repleta de livros antigos... Há todos os tipos de livros neste lugar desde livros de culinária a livros de rituais macabros. Você se pergunta o que é este lugar...'),
	nl,
	!.

describe(q_3) :-
	write('Você está em uma biblioteca repleta de livros antigos... Há todos os tipos de livros neste lugar desde livros de culinária a livros de rituais macabros. Você se pergunta o que é este lugar... Um livro está jogado ao chão e chama atenção.'),
	nl,
	!.

describe(q_4) :-
	at_item_obtained(q_4),
	write('Você se encontra em um quarto com uma grande poltrona virada em direção a um pote vazio... Bem, pelo menos tem ar dentro...'),
	nl,
	!.

describe(q_4) :-
	write('Você se encontra em um quarto com uma grande poltrona virada em direção a um pote com uma perna de lagarto... Ao chegar mais perto você nota que a perna ainda se mexe! Será que ainda está viva?'),
	nl,
	!.

describe(q_5) :-
	at_item_obtained(q_5),
	write('Você entra em um lugar repleto de armamentos medievais..."'),
	nl,
	!.

describe(q_5) :-
	write('Você entra em um lugar repleto de armamentos medievais, entre eles uma espada chama sua atenção...'),
	nl,
	!.

describe(q_6) :-
	at_item_obtained(q_6),
	write('Você está em um corredor com quatro direções...'),
	nl,
	!.

describe(q_6) :-
	write('Você está em um corredor com quatro direções...'),
	nl,
	!.

describe(q_7) :-
	at_item_obtained(q_7),
	write('"Você se encontra em um lugar onde o grupo Monty Python está encenando o \"Sapo Crocante\". Você olha pra um confeiteiro que logo lhe diz: \"se eu tirar os ossos, eles não vão ser mais crocantes, né?\" Você então vê uma caixa de chocolate onde está escrito \"Sapos Crocantes\". Ela parece conter uma dúzia de sapos inteiros, cuidadosamente cobertos pelo melhor chocolate."'),
	nl,
	!.

describe(q_7) :-
	write('"Você se encontra em um lugar onde o grupo Monty Python está encenando o \"Sapo Crocante\". Você olha pra um confeiteiro que logo lhe diz: \"se eu tirar os ossos, eles não vão ser mais crocantes, né?\" Você então vê uma caixa de chocolate onde está escrito \"Sapos Crocantes\". Ela parece conter uma dúzia de sapos inteiros, cuidadosamente cobertos pelo melhor chocolate."'),
	nl,
	!.

describe(q_8) :-
	at_item_obtained(q_8),
	write('Você está em um belo quarto com uma grande cama... Ao olhar as paredes, você nota a imagem de uma coruja sem uma das asas em um quadro... Que peculiar... e esquisito.'),
	nl,
	!.

describe(q_8) :-
	write('Você está em um belo quarto com uma grande cama... Ao olhar as paredes, você nota a imagem de uma coruja em um quadro... Que peculiar! Uma das asas deste coruja é de verdade! Você não sabe se fica impressionado ou assustado...'),
	nl,
	!.

describe(q_9) :-
	at(espada, in_pocket),
	alive(dragon),
	retract(alive(dragon)),
	write('Você está em lugar escuro e úmido... ouve um barulho, como se algo estivesse rastejando. Assustado, você se abaixa e empunha sua espada. De repente, olhos vermelhos se abrem revelando um dragão ao seu lado. Antes que ele possa lhe atacar, você desfere um golpe mortal e quase não tem não consegue se esquivar do imenso corpo que desaba no chão.\nAssim que se recupera do susto, você percebe que está de frente para a porta que leva a uma cozinha. Através do corredor, você pode ouvir alguém gritando \"Parem! parem!\" ao mesmo tempo que vários gatos miam inquietos. De repente, um barulho abrupto e o completo silêncio.'),
	nl,
	!.

describe(q_9) :-
	alive(dragon),
	verify_chance,
	write('Você está em lugar escuro e úmido... ouve um barulho, como se algo estivesse rastejando. Assustado, você se abaixa tentando entender o que está acontecendo e, de repente, vê um par de olhos na espreita. A última coisa que pensa é como seria bom se tivesse algo para se defender. Antes que possa esboçar qualquer reação, é atacado por um dragão que não lhe dá qualquer chance. Que triste fim para um herói.'),
	nl,
	die,
	!.

describe(q_9) :-
	alive(dragon),
	write('Você está em lugar escuro e úmido... ouve um barulho, como se algo estivesse rastejando. Assustado, você se abaixa tentando entender o que está acontecendo e, de repente, vê um par de olhos na espreita. Antes que o dragão possa desferir seu ataque, você salta pela porta entreaberta, fugindo do recinto."'),
	nl,
	norte,
	!.

describe(q_9) :-
	write('Você está em um lugar escuro e úmido, com o cheiro de sangue impregnado. À sua frente está a porta que leva à cozinha. Perto da porta, você pode ouvir miados distantes.'),
	nl,
	!.

describe(q_10) :-
	write('Você está em uma cozinha. Você então vê, ao fundo, um cozinheiro colocando uma porção de atum em uma panela. Apesar de não ter certeza, parece haver uma espingarda ao lado do fogão. Há pelos brancos voando por todo lado, enquanto o cozinheiro parece falar sozinho: \"impossível trabalhar com todos esses gatos aqui...\"'),
	nl,
	!.

/* Misc */

verify_chance :-
	random(0, 100, X),
	will_i_die(X),
	!.

will_i_die(X) :-
	X > 30,
	!.
