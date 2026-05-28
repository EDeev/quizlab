import random
from .models import Quiz
from django.db import transaction
from faker import Faker

fk = Faker()

def gentestdata():
    with transaction.atomic():
        for i in range(100):
            new_quiz = Quiz()
            new_quiz.title = fk.sentence(nb_words=4)
            new_quiz.description = fk.paragraph()
            new_quiz.author = fk.name()
            new_quiz.time_limit = random.randint(5, 60)
            new_quiz.is_published = random.random() > 0.5
            new_quiz.save()
    print('Сделана генерация')
