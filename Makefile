# ==========================================================================================
# Makefile - Sistema de Gestao de RH e Folha de Pagamento (Arvore B+ em Disco)
# Grupo - [nome-do-aluno-1], [nome-do-aluno-2], [nome-do-aluno-3], [nome-do-aluno-4]
# ==========================================================================================

# 1️⃣ Variáveis (para facilitar manutenção)
CFLAGS=-O0 -std=c11 -Wall -Werror -Wextra -Wno-sign-compare -Wno-unused-parameter -Wno-unused-variable -Wshadow
CLIBS=-lm
EXECUTAVEL=SistemaRH.exe # nome do executável final

# ==========================================================
# 2️⃣ Alvo principal (primeiro alvo → padrão ao rodar "make")
# ==========================================================

$(EXECUTAVEL): main.o Bplus.o Funcionario.o Util.o
	@echo "🔧 Ligando objetos e gerando executável..."
	gcc main.o Bplus.o Funcionario.o Util.o $(CLIBS) -o $(EXECUTAVEL)
	@echo "✅ Compilação concluída."
	@echo "🚀 Para executar o programa digite ./$(EXECUTAVEL) no terminal."


# ==========================================================
# 3️⃣ Regras para gerar os arquivos objeto
# ==========================================================

main.o: main.c Bplus.h Funcionario.h Util.h
	@echo "🧩 Compilando main.c"
	gcc -c $(CFLAGS) main.c

Bplus.o: Bplus.c Bplus.h
	@echo "🧩 Compilando Bplus.c"
	gcc -c $(CFLAGS) Bplus.c

Funcionario.o: Funcionario.c Funcionario.h Bplus.h
	@echo "🧩 Compilando Funcionario.c"
	gcc -c $(CFLAGS) Funcionario.c

Util.o: Util.c Util.h
	@echo "🧩 Compilando Util.c"
	gcc -c $(CFLAGS) Util.c

# ==========================================================
# 4️⃣ Alvos auxiliares (não geram arquivos → PHONY)
# ==========================================================

.PHONY: clean run memcheck

# Executa o programa (compila antes se necessário)
run: $(EXECUTAVEL)
	@echo "🚀 Executando o programa:"
	./$(EXECUTAVEL)

# Executa o programa com o valgrind (compila antes se necessário)
memcheck: $(EXECUTAVEL)
	@echo "👾 Verificando vazamento de memória com o valgrind:"
	valgrind --tool=memcheck --leak-check=full ./$(EXECUTAVEL)

# Limpa os arquivos temporários, o executável e os arquivos de dados
# binários gerados em disco pela Árvore B+ e pelo arquivo de funcionários
clean:
	@echo "🧹 Limpando arquivos compilados..."
	rm -rf *.o $(EXECUTAVEL)
	rm -f *.idx *.dat *.bin
	@echo "🧼 Diretório limpo!"
